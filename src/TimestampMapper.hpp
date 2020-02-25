#pragma once

#include "Filesystem.hpp"

#include <algorithm>
#include <cctype>

#include "CommandLine.hpp"
#include "Database.hpp"
#include "DatasetDesc.hpp"

namespace tsm {

class TimestampMapper {

public:
    explicit TimestampMapper(const CommandLine::Options& opts);

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
    [[nodiscard]] inline auto fileOrDirExists(const fs::path& path) const {
        return fs::exists(path);
    }
    ///
    [[nodiscard]] bool createDirectory(const fs::path& path) const noexcept;
    ///
    [[nodiscard]] std::vector<fs::path> createFileList() const;
    ///
    [[nodiscard]] inline auto shouldDeleteIndexFile() const noexcept {
        return m_indexFileExists && !m_cliOptions.KeepIndexFile;
    }
    ///
    void deleteIndexFile();

    const ds::DATASET_TYPE m_datasetType;
    const CommandLine::Options m_cliOptions;
    bool m_indexFileExists{ false };

    Database m_database;
};

} // namespace tsm
