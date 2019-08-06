#pragma once

#include <filesystem>
#include <algorithm>
#include <cctype>

#include "Database.hpp"
#include "DatasetDesc.hpp"

namespace tsm {

class TimestampMapper {

public:
    TimestampMapper(const std::filesystem::path& inputDir,
                    const std::filesystem::path& outputDir,
                    const std::string& datasetName,
                    const ds::DATASET_TYPE datasetType,
                    const ds::VARIABLE_LAYOUT variableLayout,
                    const bool regenIndices);

    /// Runs the tool.
    /// Returns true on success, false on failure.
    bool exec();

private:
    ///
    [[nodiscard]] inline auto sanitizeDatasetName(const std::string& datasetName) const {
        std::string res;
        res.reserve(datasetName.size());

        std::transform(datasetName.cbegin(), datasetName.cend(), res.begin(), ::tolower);

        return res;
    }
    ///
    [[nodiscard]] inline auto sanitizeDirectoryPath(const std::string& dirPath) const {
        if (dirPath.back() == '/') {
            return dirPath;
        }

        return dirPath + '/';
    }
    ///
    [[nodiscard]] inline auto fileOrDirExists(const std::filesystem::path& path) const {
        return std::filesystem::exists(path);
    }
    ///
    [[nodiscard]] bool createDirectory(const std::filesystem::path& path) const noexcept;
    ///
    [[nodiscard]] std::vector<std::filesystem::path> 
                                        createFileList(const std::filesystem::path& inputDirOrIndexFile) const;

    ///
    void deleteIndexFile();

    const std::filesystem::path m_inputDir;
    const std::filesystem::path m_outputDir;
    const std::string m_datasetName;
    const ds::DATASET_TYPE m_datasetType;
    const ds::VARIABLE_LAYOUT m_variableLayout;
    const bool m_regenIndices{ false };

    const std::filesystem::path m_filesToIndexPath;
    bool m_indexFileExists{ false };

    Database m_database;
};

} // namespace tsm
