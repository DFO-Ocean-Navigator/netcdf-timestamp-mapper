#include <ncFile.h>
#include <ncDim.h>
#include <ncVar.h>
#include <cxxopts/include/cxxopts.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>

#include "Database.hpp"

namespace fs = std::filesystem;

using timestamp_t = unsigned long long;

struct [[nodiscard]] TimestampFilenamePair {
    TimestampFilenamePair(const std::vector<std::string>& timestamps, const fs::path& path) :   Timestamps{timestamps},
                                                                                                NCFilePath{path} {}
    TimestampFilenamePair(const TimestampFilenamePair&) = default;
    TimestampFilenamePair(TimestampFilenamePair&&) = default;

    TimestampFilenamePair& operator=(const TimestampFilenamePair&) = default;
    TimestampFilenamePair& operator=(TimestampFilenamePair&&) = default;

    std::vector<std::string> Timestamps;
    fs::path NCFilePath;
};

/// Returns a list of NC file paths (if any) given an inputDirectory
/// or parses a text file with the non-indexed file list.
[[nodiscard]] auto getFiles(const fs::path& inputDirectoryOrIndexFile) {
    std::vector<fs::path> paths;

    if (inputDirectoryOrIndexFile.extension() == ".txt") {
        std::ifstream f(inputDirectoryOrIndexFile);

        if (f.is_open()) {
            const auto &directory{inputDirectoryOrIndexFile.parent_path()};
            std::string line;
            while (std::getline(f, line)) {
                const fs::path p{line};
                if (p.extension() == ".nc") {
                    paths.emplace_back(directory / p);
                }
            }
            f.close();
        }

        return paths;
    }

    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    const auto options{ fs::directory_options::follow_directory_symlink };

    for (const auto& file : recursive_directory_iterator(inputDirectoryOrIndexFile, options)) {
        if (fs::path(file).extension() == ".nc") {
            paths.emplace_back(file);
        }
    }

    return paths;
}

[[nodiscard]] auto findTimeDim(const netCDF::NcFile& ncFile) {

    for (const auto& dim : ncFile.getDims()) {
        if (dim.first.find("time") != std::string::npos) {
            return dim.first;
        }
    }

    return std::string{};
}

[[nodiscard]] auto convertNCTimeToISOExtended(const timestamp_t timestamp) {
    namespace bg = boost::gregorian;
    namespace bp = boost::posix_time;

    const bg::date epoch{ 1950, 1, 1 };
    const bp::ptime timeSinceEpoch(epoch, bp::seconds(timestamp));

    return bp::to_iso_extended_string(timeSinceEpoch);
}

[[nodiscard]] std::optional<std::vector<timestamp_t>> getTimestampValues(const fs::path& path) {

    // Open NC file
    netCDF::NcFile f{path, netCDF::NcFile::read};

    auto timeDim{ findTimeDim(f) };
    if (timeDim.empty()) {
        return std::nullopt;
    }

    // Get timestamp values
    netCDF::NcDim dim;
    netCDF::NcVar var;
    f.getCoordVar(timeDim, dim, var);

    std::vector<timestamp_t> vals(dim.getSize());
    var.getVar(vals.data());

    // NcFile destructor will close the file and release resources

    return std::make_optional(vals);
}

[[nodiscard]] std::optional<std::vector<TimestampFilenamePair>> createPairs(const std::vector<fs::path>& paths) {

    std::vector<TimestampFilenamePair> pairs;
    pairs.reserve(paths.size());

    for (const auto& path : paths) {
       
        const auto timestamps{ getTimestampValues(path) };
        if (!timestamps) {
            return std::nullopt;
        }
        const auto& vals{ timestamps.value() }; // Get underlyng vector

        // Allocate memory for iso timestamps
        std::vector<std::string> isoTimestamps(vals.size());

        // Convert raw timestamps to ISO Extended.
        std::transform(vals.cbegin(), vals.cend(), isoTimestamps.begin(), convertNCTimeToISOExtended);

        pairs.emplace_back(isoTimestamps, path);
    }
   
    return std::make_optional(pairs);
}

auto createOutputDirectory(const std::string& path) noexcept {
    std::error_code e;
    fs::create_directory(path, e);

    if (e) {
        std::cerr << e.message() << std::endl;
        return false;
    }
    return true;
}

[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv) {

    try {
        cxxopts::Options options("NetCDF DB Mapper", "One line description of MyProgram");

        options.add_options()
        ("d,input-dir", "Input directory.", cxxopts::value<std::string>())
        ("n,dataset-name", "Dataset name (no spaces).", cxxopts::value<std::string>())
        ("o,output-dir", "Output directory.", cxxopts::value<std::string>())
        ;

        const auto& result{ options.parse(argc, argv) };

        return std::make_optional(result);

    } catch(const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    } catch(...) {
        std::cerr << "Unhandled execption." << std::endl;
        return std::nullopt;
    }
}

/// Convert given dataset name to lower-case.
[[nodiscard]] auto sanitizeDatasetName(const std::string& datasetName) {
    std::string res;
    res.reserve(datasetName.size());

    std::transform(datasetName.cbegin(), datasetName.cend(), res.begin(), ::tolower);

    return res;
}

int main(int argc, char** argv) {

    const auto& result{ parseCmdLineOptions(argc, argv) };
    if (!result) {
        return EXIT_FAILURE;
    } 

    auto inputDirectory{ (*result)["input-dir"].as<std::string>() };
    // Account for directory paths with no trailing forward-slash
    if (inputDirectory.back() != '/') {
        inputDirectory.insert(inputDirectory.end(), '/');
    }
    if (!fs::exists(inputDirectory)) {
        std::cerr << "Input directory (" << inputDirectory << ") not found." << std::endl;
        return EXIT_FAILURE;
    }

    const auto& outputDirectory{(*result)["output-dir"].as<std::string>()};
    if (!createOutputDirectory(outputDirectory)) {
        std::cerr << "Failed to create output directory: " << outputDirectory << std::endl;
        return EXIT_FAILURE;
    }

    // Check if optional file containing non-indexed NC files exists.
    const auto& filesToIndex{inputDirectory + "files_to_index.txt"};
    bool indexFileExists{ false };
    if (fs::exists(filesToIndex)) {
        std::cout << "Found list of non-indexed files. Only the files contained in this list will be indexed..." << std::endl;
        indexFileExists = true;
    }
    else {
        std::cout << "List of non-indexed files not found. Continuing with complete indexing operation..." << std::endl;
    }

    const auto& filePaths{getFiles(indexFileExists ? filesToIndex : inputDirectory)};
    if (filePaths.empty()) {
        std::cout << "No .nc files found in " << inputDirectory << ".\nExiting..." << std::endl;
        return 0;
    }

    const auto& pairs{ createPairs(filePaths) };
    if (!pairs) {
        std::cerr << "Failed to find time dimension in one of the NetCDF files." << std::endl;
        return EXIT_FAILURE;
    }

    const auto& datasetName{ sanitizeDatasetName((*result)["dataset-name"].as<std::string>()) };
    
    Database db{ outputDirectory, datasetName };

    return 0;
}
