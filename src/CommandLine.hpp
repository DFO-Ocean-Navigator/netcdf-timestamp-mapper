#pragma once

#include "Filesystem.hpp"

#include <cxxopts/include/cxxopts.hpp>

#include <optional>
#include <string>

namespace tsm {

/***********************************************************************************/
class CommandLine final {

public:
    struct [[nodiscard]] Options {
        std::string InputDir;
        std::string DatasetName;
        std::string OutputDir;
        std::string RegexPattern;
        fs::path FileListPath;
        std::string RegexEngine;
        std::vector<fs::path> STDINfileList;
        bool DryRun{ false };
        bool KeepIndexFile{ false };
        bool RegenIndices{ false };
        bool Forecast{ false };
        bool Historical{ false };
    };

    CommandLine() = default;

    ///
    [[nodiscard]] bool parse(int argc, char** argv);
    ///
    [[nodiscard]] const auto getOptions() const noexcept {
        return m_options;
    }
    

private:
    [[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLine(int argc, char** argv);
    [[nodiscard]] bool verify(const cxxopts::ParseResult& result, const std::vector<fs::path>& STDINfileList);
    [[nodiscard]] std::vector<fs::path> readSTDIN() const;

    Options m_options;
};
    
} // namespace tsm
