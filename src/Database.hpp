#pragma once

#include "DeletedUniquePtr.hpp"

#include <string>
#include <filesystem>

// Forward declarations
struct sqlite3;
struct sqlite3_stmt;
namespace tsm::ds {
class DatasetDesc;
}

namespace tsm {

class Database {
    using stmtPtr = utils::deleted_unique_ptr<sqlite3_stmt>; 

public:
    
    Database(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& datasetName);
    ~Database();

    ///
    [[nodiscard]] bool open();
    ///
    void insertData(const ds::DatasetDesc& datasetDesc);

private:
    ///
    void configureSQLITE();
    ///
    void configureDBConnection();
    /// Close connection to database.
    void closeConnection();
    /// Ideal for 1-shot SQL statements (like setting PRAGMAs, etc.).
    void execStatement(const std::string& sqlStatement, int (*callback)(void*, int, char**, char**) = nullptr);
    /// Ideal for repetitive SQL statements.
    stmtPtr prepareStatement(const std::string& sqlStatement);
    ///
    void insertHistoricalCombined(const ds::DatasetDesc& datasetDesc);
    ///
    void insertHistoricalSplit(const ds::DatasetDesc& datasetDesc);
    ///
    void createHistoricalCombinedTable();
    ///
    void createHistoricalSplitTable();

    sqlite3* m_DBHandle{ nullptr };
    const std::filesystem::path m_inputPath;
    const std::filesystem::path m_outputFilePath;
};

} // namespace tsm
