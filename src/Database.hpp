#pragma once

#include <string>

// Forward declarations
struct sqlite3;
struct sqlite3_stmt;

class Database {

public:
    
    Database(const std::string& path, const std::string& datasetName);
    ~Database();

private:
    ///
    void configureSQLITE();
    ///
    void closeConnection();
    ///
    void createDatabase(const std::string& dbName);
    ///
    void execStatement(const std::string& sqlStatement);

    sqlite3* m_DBHandle{ nullptr };
};
