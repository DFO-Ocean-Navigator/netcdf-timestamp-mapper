#include "Database.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

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
    constexpr auto createTableQuery{
        "CREATE TABLE IF NOT EXISTS Timestamps ("
            "timestamp_id INT AUTO_INCREMENT PRIMARY KEY,"
            "timestamp INT NOT NULL"
        ");"
    };

    constexpr auto createTimestampIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON Timestamps(timestamp);"
    };

    constexpr auto createFilepathsTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "filepath_id INT AUTO_INCREMENT PRIMARY KEY, "
            "timestamp_id INT, "
            "filepath VARCHAR(4096) NOT NULL, "
            "FOREIGN KEY (timestamp_id) REFERENCES Timestamps(timestamp_id)"
        ");"
    };

    // Create index on foreign key
    constexpr auto createForeignKeyIndex{
        "CREATE INDEX IF NOT EXISTS idx_foreign_key on Filepaths(timestamp_id);"
    };

    execStatement(createTableQuery);
    execStatement(createTimestampIndexQuery);
    execStatement(createFilepathsTableQuery);
    execStatement(createForeignKeyIndex);
}

} // namespace tsm
