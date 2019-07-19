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
        cxxopts::Options options("NetCDF Timestamp Mapper", "Maps timestamps (and variables if needed) to netCDF files using sqlite3.");

        options.add_options()
        ("i,input-dir", "Input directory.", cxxopts::value<std::string>())
        ("n,dataset-name", "Dataset name (no spaces).", cxxopts::value<std::string>())
        ("o,output-dir", "Output directory.", cxxopts::value<std::string>())
        ("regen-indices", "Regenerate indices.", cxxopts::value<bool>())
        ("f,forecast", "Forecast dataset type.", cxxopts::value<bool>())
        ("h,historical", "Historical dataset type.", cxxopts::value<bool>())
        ("c,variables-combined", "Dataset variables are all in one file.", cxxopts::value<bool>())
        ("s,variables-split", "Dataset variables are split across multiple files.", cxxopts::value<bool>())
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
                                                                RegenIndices{ result.count("regen-indices") > 0 },
                                                                Forecast{ result.count("forecast") > 0 },
                                                                Historical{ result.count("historical") > 0 },
                                                                VarsCombined{ result.count("variables-combined") > 0 },
                                                                VarsSplit{ result.count("variables-split") > 0 } {}

    const std::string InputDir;
    const std::string DatasetName;
    const std::string OutputDir;
    const bool RegenIndices{ false };
    const bool Forecast{ false };
    const bool Historical{ false };
    const bool VarsCombined{ false};
    const bool VarsSplit{ false };
};

} // namespace tsm::cli
