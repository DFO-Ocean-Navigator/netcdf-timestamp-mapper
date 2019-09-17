#pragma once

#include <cxxopts/include/cxxopts.hpp>

#include <optional>
#include <string>
#include <iostream>

namespace tsm::cli {

/***********************************************************************************/
[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv) {

    try
    {
        cxxopts::Options options("NetCDF Timestamp Mapper", "Maps timestamps and variables to netCDF files using sqlite3.");

        options.add_options()
        ("i,input-dir", "Input directory.", cxxopts::value<std::string>())
        ("n,dataset-name", "Dataset name (no spaces).", cxxopts::value<std::string>())
        ("o,output-dir", "Output directory.", cxxopts::value<std::string>())
        ("regen-indices", "Regenerate indices.", cxxopts::value<bool>())
        ("f,forecast", "Forecast dataset type.", cxxopts::value<bool>())
        ("h,historical", "Historical dataset type.", cxxopts::value<bool>())
        ("r,regex", "Regex to apply to input directory.", cxxopts::value<std::string>())
        ("file-list", "Path to text file containing absolute file paths of netcdf files to be indexed.", cxxopts::value<std::string>())
        ("help", "Print help.")
        ;

        return std::make_optional(options.parse(argc, argv));
    }
    catch (const cxxopts::OptionException &e)
    {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }
    catch (...)
    {
        std::cerr << "Unhandled execption." << std::endl;
        return std::nullopt;
    }
}

/***********************************************************************************/
void printHelp() {

}

/***********************************************************************************/
struct [[nodiscard]] CLIOptions {
    explicit CLIOptions(const cxxopts::ParseResult& result) :   InputDir{ result["input-dir"].as<std::string>() },
                                                                DatasetName{ result["dataset-name"].as<std::string>() },
                                                                OutputDir{ result["output-dir"].as<std::string>() },
                                                                RegexPattern{ result.count("regex") > 0 ? result["regex"].as<std::string>() : ".*" },
                                                                FileListPath{ result.count("file-list") > 0 ? result["file-list"].as<std::string>() : ""},
                                                                RegenIndices{ result.count("regen-indices") > 0 },
                                                                Forecast{ result.count("forecast") > 0 },
                                                                Historical{ result.count("historical") > 0 } {}

    const std::string InputDir;
    const std::string DatasetName;
    const std::string OutputDir;
    const std::string RegexPattern;
    const std::string FileListPath;
    const bool RegenIndices{ false };
    const bool Forecast{ false };
    const bool Historical{ false };
};

} // namespace tsm::cli
