#include <ncFile.h>
#include <cxxopts/include/cxxopts.hpp>

#include <vector>
#include <iostream>
#include <optional>
#include <filesystem>

#include "Database.hpp"

namespace fs = std::filesystem;


struct [[nodiscard]] TimestampFilenamePair {
    explicit TimestampFilenamePair(const fs::path& ncFilePath) : NCFilePath{ ncFilePath } {}

    std::string Timestamp;
    fs::path NCFilePath;
};

[[nodiscard]] auto getFiles(const fs::path& inputDirectory) {
    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    const auto options{ fs::directory_options::follow_directory_symlink };

    std::vector<fs::path> paths;

    for (const auto& file : recursive_directory_iterator(inputDirectory, options)) {
        if (fs::path(file).extension() == ".nc") {
            paths.emplace_back(file);
        }
    }

        return paths;
}

auto createOutputDirectory(const std::string& path) {
    return fs::create_directory(path);
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
    res.resize(datasetName.size());

    std::transform(datasetName.cbegin(), datasetName.cend(), res.begin(), ::tolower);

    return res;
}

int main(int argc, char** argv) {

    const auto& result{ parseCmdLineOptions(argc, argv)};
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

    const auto& outputDirectory{ (*result)["output-dir"].as<std::string>() };
    /*
    if (!createOutputDirectory(outputDirectory)) {
        std::cerr << "Failed to create output directory: " << outputDirectory << std::endl;
        return EXIT_FAILURE;
    }
    */

    const auto& datasetName{ sanitizeDatasetName((*result)["dataset-name"].as<std::string>()) };

    const auto& filePaths{ getFiles(inputDirectory) };
    if (filePaths.empty()) {
        std::cout << "No .nc files found in " << inputDirectory << ". Exiting..." << std::endl;
        return 0;
    }

    Database db{ outputDirectory, datasetName };

    return 0;
}
