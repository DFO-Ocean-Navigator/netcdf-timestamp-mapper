#include "Database.hpp"

#include "DatasetDesc.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

// Required queries:
// SELECT filepath FROM Timestamps INNER JOIN Filepaths WHERE timestamp='2193091200';

namespace tsm {

/***********************************************************************************/
Database::Database(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& datasetName) : m_inputPath{inputPath},
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
        closeConnection();
        std::cerr << sqlite3_errmsg(m_DBHandle) << std::endl;
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
    return;
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
    execStatement("PRAGMA synchronous = OFF");
    execStatement("PRAGMA foreign_keys = ON;");
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

    sqlite3_prepare_v2(m_DBHandle, sqlStatement.data(), sqlStatement.length(), &stmt, nullptr);

    return stmtPtr(stmt, [](auto* stmt) { sqlite3_finalize(stmt); });
}

/***********************************************************************************/
void Database::insertHistorical(const ds::DatasetDesc& datasetDesc) {

    createHistoricalTable();

    auto insertFilePathStmt{ prepareStatement("INSERT INTO Filepaths(filepath) VALUES (@PT);") };
    auto insertVariableStmt{ prepareStatement("INSERT INTO Variables(variable) VALUES (@VS);") };
    auto insertTimestampStmt{ prepareStatement("INSERT INTO Timestamps(timestamp) VALUES (@TS);") };

    std::unordered_set<std::string> insertedVariables;
    std::unordered_set<ds::timestamp_t> insertedTimestamps;

    execStatement("BEGIN TRANSACTION");
    for (const auto& ncFile : datasetDesc.m_ncFiles) {

        // Insert filepath into its table to auto-generate the filepath_id.
        sqlite3_bind_text(&(*insertFilePathStmt), 1, ncFile.NCFilePath.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(&(*insertFilePathStmt)); // Execute statement
        sqlite3_clear_bindings(&(*insertFilePathStmt));
        sqlite3_reset(&(*insertFilePathStmt));

        // Insert variables into their table
        for (const auto& variable : ncFile.Variables) {
            if (insertedVariables.contains(variable)) { // skip already inserted variables
                continue;
            }
            sqlite3_bind_text(&(*insertVariableStmt), 1, variable.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(&(*insertVariableStmt)); // Execute statement
            sqlite3_clear_bindings(&(*insertVariableStmt));
            sqlite3_reset(&(*insertVariableStmt));
            
            insertedVariables.insert(variable);
        }

         // Insert timestamps
        for (const auto ts : ncFile.Timestamps) {
            if (insertedTimestamps.contains(ts)) {
                continue;
            }
                
            std::stringstream ss;
            ss << ts;

            sqlite3_bind_text(&(*insertTimestampStmt), 1, ss.str().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(&(*insertTimestampStmt));
            sqlite3_clear_bindings(&(*insertTimestampStmt));
            sqlite3_reset(&(*insertTimestampStmt));

            insertedTimestamps.insert(ts);
        }
    }
    execStatement("END TRANSACTION");

    populateHistoricalJoinTable(datasetDesc);
}

/***********************************************************************************/
void Database::populateHistoricalJoinTable(const ds::DatasetDesc& datasetDesc) {
    auto insertJoinTableStmt{ prepareStatement("INSERT INTO TimestampVariableFilepath(filepath_id, variable_id, timestamp_id) VALUES ((SELECT id FROM Filepaths WHERE filepath = @PT), \
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
                sqlite3_bind_text(&(*insertJoinTableStmt), 2, variable.c_str(), -1, SQLITE_TRANSIENT);
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
void Database::createHistoricalTable() {
    const auto createFilepathsTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "id INTEGER PRIMARY KEY, "
            "filepath TEXT NOT NULL"
        ");"
    };

    const auto createVariablesTableQuery{
        "CREATE TABLE IF NOT EXISTS Variables ("
            "id INTEGER PRIMARY KEY,"
            "variable TEXT UNIQUE NOT NULL"
        ");"
    };

    const auto createTimestampTableQuery{
        "CREATE TABLE IF NOT EXISTS Timestamps ("
            "id INTEGER PRIMARY KEY,"
            "timestamp INTEGER UNIQUE NOT NULL"
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
    execStatement(createVariablesTableQuery);
    execStatement(createTimestampTableQuery);
    execStatement(createJoinTableQuery);
    execStatement(createForeignKeyIndexFPQuery);
    execStatement(createForeignKeyIndexVarQuery);
    execStatement(createForeignKeyTimestampIndexQuery);
    execStatement(createTimestampIndexQuery);
    execStatement(createFilePathIndexQuery);
}

} // namespace tsm
