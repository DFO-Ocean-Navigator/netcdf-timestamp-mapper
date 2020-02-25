#include "CommandLine.hpp"

#include <iostream>
#include <unordered_set>

namespace tsm {

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
std::string cleanRegexPattern(const std::string& inputPattern) {
    std::string cleaned = inputPattern;
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\''), cleaned.end());

    return cleaned;
}

/***********************************************************************************/
bool CommandLine::parse(int argc, char** argv) {

    const auto& inputFiles{ readSTDIN() };
    
    auto result{ parseCmdLine(argc, argv) };
    if (!result) {
        return false;
    }

    if (!verify(*result, inputFiles)) {
        return false;
    }

    return true;
}

/***********************************************************************************/
std::optional<cxxopts::ParseResult> CommandLine::parseCmdLine(int argc, char** argv) {
    try {
        
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
    catch (const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }
    catch (...) {
        std::cerr << "Unhandled execption when parsing command-line options." << std::endl;
        return std::nullopt;
    }
}

/***********************************************************************************/
bool CommandLine::verify(const cxxopts::ParseResult& result, const std::vector<fs::path> STDINfileList) {

    m_options.InputDir = result.count("input-dir") > 0 ? sanitizeDirectoryPath(result["input-dir"].as<std::string>()) : "";
    m_options.DatasetName = result.count("dataset-name") > 0 ? result["dataset-name"].as<std::string>() : "";
    m_options.OutputDir = result.count("output-dir") > 0 ? sanitizeDirectoryPath(result["output-dir"].as<std::string>()) : "";
    m_options.RegexPattern = result.count("regex") > 0 ? cleanRegexPattern(result["regex"].as<std::string>()) : ".*";
    m_options.FileListPath = result.count("file-list") > 0 ? result["file-list"].as<std::string>() : "";
    m_options.RegexEngine = result.count("regex-engine") > 0 ? result["regex-engine"].as<std::string>() : "egrep";
    m_options.STDINfileList = STDINfileList;
    m_options.DryRun = result.count("dry-run") > 0;
    m_options.KeepIndexFile = result.count("keep-file-list") > 0;
    m_options.RegenIndices = result.count("regen-indices") > 0;
    m_options.Forecast = result.count("forecast") > 0;
    m_options.Historical = result.count("historical") > 0;
    
    if (m_options.DatasetName.empty()) {
        std::cerr << "Dataset name is required. Use -n or --dataset-name to specify." << std::endl;
        return false;
    }

    if (m_options.InputDir.empty() && m_options.FileListPath.empty() && m_options.STDINfileList.empty()) {
        std::cerr << "Input directory, file list path, or file list via STDIN were not given. One is required. Use --input-dir, --file-list, or | to specify." << std::endl;
        return false;
    }

    if (m_options.OutputDir.empty()) {
        std::cerr << "Output directory is required. Use -o or --output-dir to specify." << std::endl;
        return false;
    }

    if (m_options.Forecast == m_options.Historical) {
        std::cerr << "ONE of --forecast OR --historical is required." << std::endl;
        return false;
    }

    const std::unordered_set<std::string> regexEngines{ "egrep", "basic", "extended", "grep", "awk", "ecmascript" };
    if (regexEngines.count(m_options.RegexEngine) == 0) {
        std::cerr << "The specified regex engine is not supported. Use --help flag to list what's available." << std::endl;
        return false;
    }

    return true;
}

/***********************************************************************************/
std::vector<fs::path> CommandLine::readSTDIN() const {
    std::vector<fs::path> files;

    for (std::string line; std::getline(std::cin, line); ) {
        files.emplace_back(line);
    }

    return files;
}

} // namespace tsm
