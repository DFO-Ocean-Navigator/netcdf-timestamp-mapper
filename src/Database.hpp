#pragma once

#include <string>
#include <filesystem>

// Forward declarations
struct sqlite3;
struct sqlite3_stmt;

namespace tsm {

class Database {

public:
    
    Database(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& datasetName);
    ~Database();

    [[nodiscard]] bool open();

private:
    ///
    void configureSQLITE();
    ///
    void closeConnection();
    ///
    void execStatement(const std::string& sqlStatement);
    /// Creates a table that maps timestamps to file names 1:1.
    void createOneToOneTable();
    ///
    void createManyToOneTable();

    sqlite3* m_DBHandle{ nullptr };
    const std::filesystem::path m_inputPath;
    const std::filesystem::path m_outputFilePath;
};

} // namespace tsm
