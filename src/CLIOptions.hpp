#pragma once

#include <cxxopts/include/cxxopts.hpp>

#include <optional>
#include <string>
#include <iostream>

namespace tsm::cli {

/***********************************************************************************/
///
[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv);

/***********************************************************************************/
/// Print help message.
void printHelp();

/***********************************************************************************/
struct [[nodiscard]] CLIOptions {
    explicit CLIOptions(const cxxopts::ParseResult& result) :   InputDir{ result["input-dir"].as<std::string>() },
                                                                DatasetName{ result["dataset-name"].as<std::string>() },
                                                                OutputDir{ result["output-dir"].as<std::string>() },
                                                                RegexPattern{ result.count("regex") > 0 ? result["regex"].as<std::string>() : ".*" },
                                                                FileListPath{ result.count("file-list") > 0 ? result["file-list"].as<std::string>() : ""},
                                                                DryRun{ result.count("dry-run") > 0},
                                                                RegenIndices{ result.count("regen-indices") > 0 },
                                                                Forecast{ result.count("forecast") > 0 },
                                                                Historical{ result.count("historical") > 0 } {}

    /// Validate the given inputs.
    [[nodiscard]] bool verify() const;

    const std::string InputDir;
    const std::string DatasetName;
    const std::string OutputDir;
    const std::string RegexPattern;
    const std::string FileListPath;
    const bool DryRun{ false };
    const bool RegenIndices{ false };
    const bool Forecast{ false };
    const bool Historical{ false };
};

} // namespace tsm::cli
