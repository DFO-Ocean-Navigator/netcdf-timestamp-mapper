#include "CLIOptions.hpp"

#include "Filesystem.hpp"

#include <iostream>
#include <unordered_set>

namespace tsm::cli {

/***********************************************************************************/
[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv) {

    try
    {
        cxxopts::Options options("NetCDF Timestamp Mapper", "Maps timestamps and variables to netCDF files using sqlite3.");

        options.allow_unrecognised_options().add_options()
        ("i,input-dir", "Input directory of netcdf files to scan.", cxxopts::value<std::string>())
        ("n,dataset-name", "Dataset name (no spaces). Will also become the filename of the resulting database (with the .sqlite3 extension).", cxxopts::value<std::string>())
        ("o,output-dir", "Output directory of the resulting database file. Make sure the user running the process has write priveleges to this folder!", cxxopts::value<std::string>())
        ("regen-indices", "Regenerate indices.")
        ("regex-engine", "Which regex engine to use: egrep (default), basic, extended, grep, awk, ecmascript.", cxxopts::value<std::string>())
        ("f,forecast", "Forecast dataset type. INACTIVE AT THIS TIME.", cxxopts::value<bool>())
        ("h,historical", "Indicates the dataset is historical in nature (i.e. not a forecast). In the future, there will be a -f flag to denote forecasts.")
        ("r,regex", "Apply a regex pattern to the input directory to filter the scanned netcdf files.", cxxopts::value<std::string>())
        ("file-list", "File containing absolute paths to netcdf files to be indexed. The format is 1 path per line (no line-ending commas, etc). Supported file extensions are: .txt, .diff, .ll.", cxxopts::value<std::string>())
        ("keep-file-list", "Don't delete the given file list after indexing.")
        ("dry-run", "Perform a dry-run of the tool; the list of scanned netcdf files will be output to the screen. No database changes will be made.")
        ("help", "Print help.")
        ;

        auto result{ options.parse(argc, argv) };

        if (result.count("help")) {
            std::cout << options.help({""}) << std::endl;
            return std::nullopt;
        }

        return std::make_optional(result);
    }
    catch (const cxxopts::OptionException &e)
    {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }
    catch (...)
    {
        std::cerr << "Unhandled execption with parsing command-line options." << std::endl;
        return std::nullopt;
    }
}

/***********************************************************************************/
std::string cleanRegexPattern(const std::string& inputPattern) {
    std::string cleaned = inputPattern;
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\''), cleaned.end());

    return cleaned;
}

/***********************************************************************************/
bool CLIOptions::verify() const {

    if (DatasetName.empty()) {
        std::cerr << "Dataset name is required. Use -n or --dataset-name to specify." << std::endl;
        return false;
    }

    if ( (InputDir.empty() || !fs::is_directory(InputDir)) && (FileListPath.empty() || !fs::exists(FileListPath)) ) {
        std::cerr << "Input directory and file list path were not given. One is required. Use --input-dir or --file-list to specify." << std::endl;
        return false;
    }

    if (OutputDir.empty()) {
        std::cerr << "Output directory is required. Use -o or --output-dir to specify." << std::endl;
        return false;
    }

    if (Forecast == Historical) {
        std::cerr << "ONE of --forecast OR --historical is required." << std::endl;
        return false;
    }

    const std::unordered_set<std::string> regexEngines{ "egrep", "basic", "extended", "grep", "awk", "ecmascript" };
    if (regexEngines.count(RegexEngine) == 0) {
        std::cerr << "The specified regex engine is not supported. Use --help flag to list what's available." << std::endl;
        return false;
    }

    return true;
}

} // namespace tsm::cli
