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
    void execStatement(const std::string& sqlStatement);
    /// Creates a table that maps timestamps to file names 1:1.
    void createOneToOneTable();

    sqlite3* m_DBHandle{ nullptr };
};
