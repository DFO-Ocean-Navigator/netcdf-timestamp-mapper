#include "Database.hpp"

#include "DatasetUitls.hpp"

#include <sqlite3.h>
#include <boost/format.hpp>

#include <stdexcept>
#include <filesystem>
#include <iostream>

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

    createManyToOneTable();

    return true;
}

/***********************************************************************************/
void Database::insertData(const std::vector<ds::TimestampFilenamePair>& data) {

    for (const auto& pair : data) {
        // Insert filepath into its table to auto-generate the filepath_id.
        const auto& insertFilePathQuery{ 
            (boost::format("INSERT INTO Filepaths(filepath) VALUES ('%s');") % pair.NCFilePath.string()).str()
        };
        execStatement(insertFilePathQuery);

        // Now insert timestamp into its table and extract the above generated filepath_id as the foreign key.
        // TODO: reduce lookups by selecting the filepath_id before looping. Need to refactor execStatement to accept an optional callback which
        // will store the resulting value.
        for (const auto ts : pair.Timestamps) {
            const auto& insertTimestampQuery{
                (boost::format("INSERT INTO Timestamps(filepath_id, timestamp) VALUES ((SELECT filepath_id FROM Filepaths WHERE filepath = '%s'), %d);")
                                                                                                                % pair.NCFilePath.string()
                                                                                                                % ts).str()
            };
            execStatement(insertTimestampQuery);
        }
    }
}

/***********************************************************************************/
void Database::insertData() {

}

/***********************************************************************************/
void Database::configureSQLITE() {
    sqlite3_config(SQLITE_CONFIG_LOG, [](void* pArg, int iErrCode, const char* zMsg) {
        std::cerr << iErrCode << " " << zMsg << std::endl;
    });
}

/***********************************************************************************/
void Database::closeConnection() {
    if (m_DBHandle) {
        sqlite3_close(m_DBHandle);
        m_DBHandle = nullptr;
    }
}

/***********************************************************************************/
void Database::execStatement(const std::string& sqlStatement) {
    
    char* errorMsg{ nullptr };
    sqlite3_exec(m_DBHandle,
                    sqlStatement.c_str(),
                    nullptr,
                    nullptr,
                    &errorMsg
    );

    if (errorMsg) {
        std::cerr << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
}

/***********************************************************************************/
void Database::createOneToOneTable() {
    constexpr auto createTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "timestamp_id INT AUTO_INCREMENT PRIMARY KEY, "
            "timestamp INT NOT NULL, "
            "filepath VARCHAR(4096) NOT NULL, "
        ");"
    };

    constexpr auto createIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON Filepaths(timestamp);"
    };

    execStatement(createTableQuery);
    execStatement(createIndexQuery);
}

/***********************************************************************************/
void Database::createManyToOneTable() {
    constexpr auto createTimestampTableQuery{
        "CREATE TABLE IF NOT EXISTS Timestamps ("
            "timestamp_id INT AUTO_INCREMENT PRIMARY KEY,"
            "filepath_id INT, "
            "timestamp INT NOT NULL, "
            "FOREIGN KEY (filepath_id) REFERENCES Filepaths(filepath_id)"
        ");"
    };

    constexpr auto createTimestampIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON Timestamps(timestamp);"
    };

    constexpr auto createForeignKeyIndex{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key on Timestamps(filepath_id);"
    };

    constexpr auto createFilepathsTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "filepath_id INT AUTO_INCREMENT PRIMARY KEY, "
            "filepath VARCHAR(4096) NOT NULL"
        ");"
    };

    execStatement(createFilepathsTableQuery);
    execStatement(createTimestampTableQuery);
    execStatement(createTimestampIndexQuery);
    execStatement(createForeignKeyIndex);
}

} // namespace tsm
