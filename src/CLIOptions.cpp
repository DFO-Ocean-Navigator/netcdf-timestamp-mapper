#include "CLIOptions.hpp"


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
        ("dry-run", "Dry run", cxxopts::value<bool>())
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
        std::cerr << "Unhandled execption with parsing command-line options." << std::endl;
        return std::nullopt;
    }
}

/***********************************************************************************/
void printHelp() {

}

/***********************************************************************************/
bool CLIOptions::verify() const {

    if (DatasetName.empty()) {
        std::cerr << "Dataset name is required. Use -n or --dataset-name to specify." << std::endl;
        return false;
    }

    if (InputDir.empty() && FileListPath.empty()) {
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

    return true;
}

} // namespace tsm::cli
