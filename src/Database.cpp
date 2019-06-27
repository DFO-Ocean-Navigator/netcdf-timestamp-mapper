#include "Database.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Database::Database(const std::string& path, const std::string& datasetName) {

    configureSQLITE();

    const auto& filepath{ fs::path(path + datasetName + ".sqlite3") } ;

    const auto result { sqlite3_open_v2(filepath.c_str(),
                                        &m_DBHandle,
                                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                        nullptr)
                        };
    if (result != SQLITE_OK) {
        closeConnection();
        throw std::runtime_error(sqlite3_errmsg(m_DBHandle));
    }

    createOneToOneTable();
}

Database::~Database() {
    closeConnection();
}

void Database::configureSQLITE() {
    sqlite3_config(SQLITE_CONFIG_LOG, [](void* pArg, int iErrCode, const char* zMsg) {
        std::cerr << iErrCode << " " << zMsg << std::endl;
    });
}

void Database::closeConnection() {
    if (m_DBHandle) {
        sqlite3_close(m_DBHandle);
    }
}

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

void Database::createOneToOneTable() {
    constexpr auto createTableQuery{
        "CREATE TABLE IF NOT EXISTS Filepaths ("
            "timestamp DATETIME NOT NULL, "
            "filepath VARCHAR(4096) NOT NULL, "
            "PRIMARY KEY(timestamp)"
        ");"
    };

    constexpr auto createIndexQuery{
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON Filepaths(timestamp);"
    };

    execStatement(createTableQuery);
    execStatement(createIndexQuery);
}
