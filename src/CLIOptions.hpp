#pragma once

#include <cxxopts/include/cxxopts.hpp>

#include <optional>
#include <string>

namespace tsm::cli {

/***********************************************************************************/
///
[[nodiscard]] inline auto sanitizeDirectoryPath(const std::string& dirPath) {
    if (dirPath.back() == '/') {
        return dirPath;
    }

    return dirPath + '/';
}

/***********************************************************************************/
///
[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv);

/***********************************************************************************/
std::string cleanRegexPattern(const std::string& inputPattern);

/***********************************************************************************/
struct [[nodiscard]] CLIOptions {
    
    CLIOptions() = default;
    
    explicit CLIOptions(const cxxopts::ParseResult& result) :   InputDir{ result.count("input-dir") > 0 ? sanitizeDirectoryPath(result["input-dir"].as<std::string>()) : "" },
                                                                DatasetName{ result.count("dataset-name") > 0 ? result["dataset-name"].as<std::string>() : "" },
                                                                OutputDir{ result.count("output-dir") > 0 ? sanitizeDirectoryPath(result["output-dir"].as<std::string>()) : "" },
                                                                RegexPattern{ result.count("regex") > 0 ? cleanRegexPattern(result["regex"].as<std::string>()) : ".*" },
                                                                FileListPath{ result.count("file-list") > 0 ? result["file-list"].as<std::string>() : ""},
                                                                RegexEngine{ result.count("regex-engine") > 0 ? result["regex-engine"].as<std::string>() : "egrep" },
                                                                DryRun{ result.count("dry-run") > 0 },
                                                                KeepIndexFile{ result.count("keep-file-list") > 0 },
                                                                RegenIndices{ result.count("regen-indices") > 0 },
                                                                Forecast{ result.count("forecast") > 0 },
                                                                Historical{ result.count("historical") > 0 } {}

    /// Validate the given inputs.
    [[nodiscard]] bool verify() const;

    std::string InputDir;
    std::string DatasetName;
    std::string OutputDir;
    std::string RegexPattern{ ".*" };
    std::string FileListPath;
    std::string RegexEngine{ "egrep" };
    bool DryRun{ false };
    bool KeepIndexFile{ false };
    bool RegenIndices{ false };
    bool Forecast{ false };
    bool Historical{ false };
};

} // namespace tsm::cli
