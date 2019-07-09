#pragma once

#include "DatasetUitls.hpp"

#include <string>
#include <filesystem>

// Forward declarations
struct sqlite3;
struct sqlite3_stmt;
namespace tsm::ds {
struct TimestampFilenamePair;
}

namespace tsm {

class Database {

public:
    
    Database(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& datasetName);
    ~Database();

    ///
    [[nodiscard]] bool open();
    ///
    void insertData(const std::vector<ds::TimestampFilenamePair>& data);
    ///
    void insertData();

private:
    ///
    void configureSQLITE();
    /// Close connection to database.
    void closeConnection();
    ///
    void execStatement(const std::string& sqlStatement, int (*callback)(void*, int, char**, char**) = nullptr);
    /// Creates a table that maps timestamps to file names 1:1.
    void createOneToOneTable();
    ///
    void createManyToOneTable();

    sqlite3* m_DBHandle{ nullptr };
    const std::filesystem::path m_inputPath;
    const std::filesystem::path m_outputFilePath;
};

} // namespace tsm
