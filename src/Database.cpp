#include "Database.hpp"

#include "DatasetDesc.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <iostream>
#include <unordered_set>

// Required queries:
// SELECT filepath FROM Timestamps INNER JOIN Filepaths WHERE timestamp='2193091200';

namespace tsm {

/***********************************************************************************/
/// Convert numeric value to std::string properly
template<typename T,
        typename = typename std::enable_if_t<std::is_arithmetic_v<T>, T>
        >
auto toString(const T numeric) {
    std::stringstream ss;
    ss << numeric;

    return ss.str();
}

/***********************************************************************************/
Database::Database(const fs::path& outputPath, const std::string& datasetName) :
                                                                                m_outputFilePath{ outputPath / (datasetName + ".sqlite3")} {

    configureSQLITE();
}

/***********************************************************************************/
Database::~Database() {
    closeConnection();
}

/***********************************************************************************/
bool Database::open() {

    const auto result { sqlite3_open_v2(m_outputFilePath.c_str(),
                                        &m_DBHandle,
                                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                        nullptr)
                        };
    if (result != SQLITE_OK) {
        std::cerr << sqlite3_errmsg(m_DBHandle) << std::endl;
        closeConnection();
        return false;
    }

    configureDBConnection();

    return true;
}

/***********************************************************************************/
void Database::insertData(const ds::DatasetDesc& datasetDesc) {

    if (datasetDesc.isHistorical()) {
        insertHistorical(datasetDesc);
        return;
    }

    if (datasetDesc.isForecast()) {
        return;
    }

    std::cout << "Nothing done." << std::endl;
}

/***********************************************************************************/
void Database::configureSQLITE() {
    sqlite3_config(SQLITE_CONFIG_LOG, [](void* pArg, int iErrCode, const char* zMsg) {
        std::cerr << "SQLITE Error: "  << iErrCode << " " << zMsg << std::endl;
    });
}

/***********************************************************************************/
void Database::configureDBConnection() {
    execStatement("PRAGMA journal_mode = MEMORY");
    execStatement("PRAGMA temp_store = MEMORY");
    execStatement("PRAGMA synchronous = OFF");
    execStatement("PRAGMA foreign_keys = ON;");
    execStatement("PRAGMA locking_mode = EXCLUSIVE");
}

/***********************************************************************************/
void Database::closeConnection() {
    if (m_DBHandle) {
        execStatement("PRAGMA optimize");
        sqlite3_close(m_DBHandle);
        m_DBHandle = nullptr;
    }
}

/***********************************************************************************/
void Database::execStatement(const std::string& sqlStatement, int (*callback)(void *, int, char **, char **) /* = nullptr */) {

    char* errorMsg{ nullptr };
    sqlite3_exec(m_DBHandle,
                 sqlStatement.c_str(),
                 callback,
                 nullptr,
                 &errorMsg);

    if (errorMsg) {
        std::cerr << "SQLITE Error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
}

/***********************************************************************************/
Database::stmtPtr Database::prepareStatement(const std::string& sqlStatement) {
    sqlite3_stmt* stmt{ nullptr };

    const auto res{ 
        sqlite3_prepare_v2(m_DBHandle, sqlStatement.data(), sqlStatement.length(), &stmt, nullptr)
    };

    if (res != SQLITE_OK) {
        std::cerr << "Error preparing SQL statement: " << sqlStatement << ".\n Error code " << res << '\n';
        printErrorMsg();
        std::cerr << "https://www.sqlite.org/c3ref/c_abort.html" << '\n';
    }

    return stmtPtr(stmt, [](auto* s) { sqlite3_finalize(s); });
}

/***********************************************************************************/
void Database::insertHistorical(const ds::DatasetDesc& datasetDesc) {

    createHistoricalTable();
    auto insertFilePathStmt{ prepareStatement("INSERT OR IGNORE INTO Filepaths(filepath) VALUES (@PT);") };
    auto insertVariableStmt{ prepareStatement("INSERT OR IGNORE INTO Variables(variable, units, longName, validMin, validMax) VALUES (@VS, @UT, @LN, @VN, @VX);") };
    auto insertTimestampStmt{ prepareStatement("INSERT OR IGNORE INTO Timestamps(timestamp) VALUES (@TS);") };
    auto insertDimStmt{ prepareStatement("INSERT OR IGNORE INTO Dimensions(name) VALUES (@DM);") };

    std::unordered_set<ds::timestamp_t> insertedTimestamps;
    std::unordered_set<std::string> insertedDimensions;
    std::unordered_set<ds::VariableDesc> insertedVariables;

    execStatement("BEGIN TRANSACTION");
    for (const auto& ncFile : datasetDesc.m_ncFiles) {

        // Insert filepath into its table to auto-generate the filepath_id.
        sqlite3_bind_text(&(*insertFilePathStmt), 1, ncFile.NCFilePath.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(&(*insertFilePathStmt)); // Execute statement
        sqlite3_clear_bindings(&(*insertFilePathStmt));
        sqlite3_reset(&(*insertFilePathStmt));

        // Insert variables into their table
        for (const auto& variable : ncFile.Variables) {
            if (insertedVariables.count(variable) > 0) { // skip already inserted variables
                continue;
            }
            sqlite3_bind_text(&(*insertVariableStmt), 1, variable.Name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(&(*insertVariableStmt), 2, variable.Units.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(&(*insertVariableStmt), 3, variable.LongName.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(&(*insertVariableStmt), 4, toString(variable.ValidMin).c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(&(*insertVariableStmt), 5, toString(variable.ValidMax).c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(&(*insertVariableStmt)); // Execute statement
            sqlite3_clear_bindings(&(*insertVariableStmt));
            sqlite3_reset(&(*insertVariableStmt));

            for (const auto& dim : variable.Dimensions) {
                if (insertedDimensions.count(dim) > 0) {
                    continue;
                }
                sqlite3_bind_text(&(*insertDimStmt), 1, dim.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(&(*insertDimStmt)); // Execute statement
                sqlite3_clear_bindings(&(*insertDimStmt));
                sqlite3_reset(&(*insertDimStmt));

                insertedDimensions.insert(dim);
            }
            
            insertedVariables.insert(variable);
        }

         // Insert timestamps
        for (const auto ts : ncFile.Timestamps) {
            if (insertedTimestamps.count(ts) > 0) {
                continue;
            }

            sqlite3_bind_text(&(*insertTimestampStmt), 1, toString(ts).c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(&(*insertTimestampStmt));
            sqlite3_clear_bindings(&(*insertTimestampStmt));
            sqlite3_reset(&(*insertTimestampStmt));

            insertedTimestamps.insert(ts);
        }
    }
    execStatement("END TRANSACTION");

    populateHistoricalJoinTable(datasetDesc);
    populateVarsDimTable(insertedVariables);
}

/***********************************************************************************/
void Database::createDimensionsTable() {
    const auto createDimsTableQuery{
        "CREATE TABLE IF NOT EXISTS Dimensions ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT UNIQUE NOT NULL ON CONFLICT IGNORE"
        ");"
    };

    execStatement(createDimsTableQuery);
}

/***********************************************************************************/
void Database::createVariablesTable() {
    const auto createVariablesTableQuery{
        "CREATE TABLE IF NOT EXISTS Variables ("
            "id INTEGER PRIMARY KEY,"
            "variable TEXT UNIQUE NOT NULL ON CONFLICT IGNORE, "
            "units TEXT, "
            "longName TEXT, "
            "validMin REAL, "
            "validMax REAL"
        ");"
    };

    execStatement(createVariablesTableQuery);
}

/***********************************************************************************/
void Database::createVariablesDimensionsTable() {
    const auto createVarsDimsTable{
        "CREATE TABLE IF NOT EXISTS VarsDims ("
            "variable_id INTEGER, "
            "dim_id INTEGER, "
            "FOREIGN KEY(variable_id) REFERENCES Variables(id),"
            "FOREIGN KEY(dim_id) REFERENCES Dimensions(id),"
            "PRIMARY KEY(variable_id, dim_id)"
        ");"
    };

    const auto createForeignKeyIndexVarQuery{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key_vardim_var ON VarsDims(variable_id);"
    };

    const auto createForeignKeyIndexDimQuery{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key_vardim_dim ON VarsDims(dim_id);"
    };

    execStatement(createVarsDimsTable);
    execStatement(createForeignKeyIndexDimQuery);
    execStatement(createForeignKeyIndexVarQuery);
}

/***********************************************************************************/
void Database::populateHistoricalJoinTable(const ds::DatasetDesc& datasetDesc) {
    auto insertJoinTableStmt{ prepareStatement("INSERT OR IGNORE INTO TimestampVariableFilepath(filepath_id, variable_id, timestamp_id) VALUES ((SELECT id FROM Filepaths WHERE filepath = @PT), \
                                                                                                                                    (SELECT id FROM Variables WHERE variable = @VR), \
                                                                                                                                    (SELECT id from Timestamps WHERE timestamp = @TS)); \
                                                ")};

    execStatement("BEGIN TRANSACTION");

    for (const auto& ncFile : datasetDesc.m_ncFiles) {
        for (const auto& variable : ncFile.Variables) {
            for (const auto ts : ncFile.Timestamps) {
                std::stringstream ss;
                ss << ts;

                sqlite3_bind_text(&(*insertJoinTableStmt), 1, ncFile.NCFilePath.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(&(*insertJoinTableStmt), 2, variable.Name.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(&(*insertJoinTableStmt), 3, ss.str().c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_step(&(*insertJoinTableStmt)); // Execute statement
                sqlite3_clear_bindings(&(*insertJoinTableStmt));
                sqlite3_reset(&(*insertJoinTableStmt));
            }
        }
    }

    execStatement("END TRANSACTION");
}

/***********************************************************************************/
void Database::populateVarsDimTable(const std::unordered_set<ds::VariableDesc>& insertedVariables) {

    auto insertStmt{ prepareStatement("INSERT OR IGNORE INTO VarsDims(variable_id, dim_id) VALUES ((SELECT id from Variables WHERE variable = @VR), \
                                                                                        (SELECT id FROM Dimensions WHERE name = @DM) ); \
                                        ") };

    execStatement("BEGIN TRANSACTION");

    for (const auto& var : insertedVariables) {
        for (const auto& dim : var.Dimensions) {
            sqlite3_bind_text(&(*insertStmt), 1, var.Name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(&(*insertStmt), 2, dim.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(&(*insertStmt)); // Execute statement
            sqlite3_clear_bindings(&(*insertStmt));
            sqlite3_reset(&(*insertStmt));
        }
    }

    execStatement("END TRANSACTION");
}

/***********************************************************************************/
void Database::createHistoricalTable() {
    createDimensionsTable();
    createVariablesTable();
    createVariablesDimensionsTable();

    const auto createFilepathsTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "id INTEGER PRIMARY KEY, "
            "filepath TEXT UNIQUE NOT NULL ON CONFLICT IGNORE"
        ");"
    };

    const auto createTimestampTableQuery{
        "CREATE TABLE IF NOT EXISTS Timestamps ("
            "id INTEGER PRIMARY KEY,"
            "timestamp INTEGER UNIQUE NOT NULL ON CONFLICT IGNORE"
        ");"
    };

    const auto createJoinTableQuery{
        "CREATE TABLE IF NOT EXISTS TimestampVariableFilepath ("
            "filepath_id INTEGER, "
            "variable_id INTEGER, "
            "timestamp_id INTEGER, "
            "FOREIGN KEY (timestamp_id) REFERENCES Timestamps(id), "
            "FOREIGN KEY (filepath_id) REFERENCES Filepaths(id), "
            "FOREIGN KEY (variable_id) REFERENCES Variables(id), "
            "PRIMARY KEY(filepath_id, variable_id, timestamp_id)"
        ");"
    };

    const auto createForeignKeyIndexFPQuery{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key_fp on TimestampVariableFilepath(timestamp_id);"
    };

    const auto createForeignKeyIndexVarQuery{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key_var on TimestampVariableFilepath(variable_id);"
    };

    const auto createForeignKeyTimestampIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key_time ON TimestampVariableFilepath(timestamp_id);"
    };

    const auto createTimestampIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON Timestamps(timestamp);"
    };

    const auto createFilePathIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_filepath ON Filepaths(filepath);"
    };

    // No need to create an index on the Variables.variable column since it's
    // always very small (i.e. < 30 rows).

    execStatement(createFilepathsTableQuery);
    execStatement(createTimestampTableQuery);
    execStatement(createJoinTableQuery);
    execStatement(createForeignKeyIndexFPQuery);
    execStatement(createForeignKeyIndexVarQuery);
    execStatement(createForeignKeyTimestampIndexQuery);
    execStatement(createTimestampIndexQuery);
    execStatement(createFilePathIndexQuery);
}

/***********************************************************************************/
void Database::printErrorMsg() {
    std::cerr << sqlite3_errmsg(m_DBHandle) << std::endl;
}

} // namespace tsm
