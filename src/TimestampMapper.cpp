#include "TimestampMapper.hpp"

#include "DatasetDesc.hpp"
#include "CrawlDirectory.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <regex>

namespace fs = std::filesystem;

namespace tsm {

/***********************************************************************************/
TimestampMapper::TimestampMapper(const cli::CLIOptions& opts) : m_cliOptions{ opts },
                                                        m_datasetType{ m_cliOptions.Forecast ? tsm::ds::DATASET_TYPE::FORECAST : tsm::ds::DATASET_TYPE::HISTORICAL },
                                                        m_indexFileExists{ fileOrDirExists(m_cliOptions.FileListPath) },
                                                        m_database{ m_cliOptions.InputDir, m_cliOptions.OutputDir, m_cliOptions.DatasetName }
{
}

/***********************************************************************************/
bool TimestampMapper::exec() {
    if (m_cliOptions.DryRun) {
        std::cout << "---DRY RUN---\n";
    }

    if (!fileOrDirExists(m_cliOptions.InputDir)) {
        std::cerr << "Input directory " << m_cliOptions.InputDir << " does not exist." << std::endl;
        return false;
    }

    if (!createDirectory(m_cliOptions.OutputDir)) {
        std::cerr << "Failed to create output directory " << m_cliOptions.OutputDir << std::endl;
        return false;
    }

    if (m_indexFileExists) {
        std::cout << "Found list of non-indexed files. Only the files contained in this list will be indexed..." << std::endl;
    }
    else {
        std::cout << "List of non-indexed files not found. Continuing with complete indexing operation..." << std::endl;
    }

    std::cout << "Creating list of all .nc files in " << (m_indexFileExists ? m_cliOptions.FileListPath : m_cliOptions.InputDir) << "..." << std::endl;
    const auto& filePaths{ createFileList(m_indexFileExists ? m_cliOptions.FileListPath : m_cliOptions.InputDir, m_cliOptions.RegexPattern) };
    if (filePaths.empty()) {
        std::cout << "No .nc files found." << "\nExiting..." << std::endl;
        return false;
    }

    if (m_cliOptions.DryRun) {
        std::copy(filePaths.cbegin(), filePaths.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));
        std::cout << "Total files found: " << filePaths.size() << '\n';
        return true;
    }

    std::cout << "Building dataset description from  " << filePaths.size() << " .nc file(s)." << std::endl;
    const ds::DatasetDesc datasetDesc{ filePaths, m_datasetType };
    if (!datasetDesc) {
        std::cerr << "Failed to find the time dimension in any of the NetCDF files." << std::endl;
        return false;
    }

    std::cout << "Opening database..." << std::endl;
    if (!m_database.open()) {
        std::cerr << "Failed to open sqlite database." << std::endl;
        return false;
    }

    std::cout << "Inserting new values into database..." << std::endl;
    m_database.insertData(datasetDesc);

    if (shouldDeleteIndexFile()) {
        std::cout << "Deleting index file." << std::endl;
        deleteIndexFile();
    }

    std::cout << "All done." << std::endl;

    return true;
}

/***********************************************************************************/
bool TimestampMapper::createDirectory(const std::filesystem::path& path) const noexcept {
    std::error_code e;
    fs::create_directory(path, e);

    if (e) {
        std::cerr << e.message() << std::endl;
        return false;
    }
    return true;
}

/***********************************************************************************/
std::vector<fs::path> TimestampMapper::createFileList(const std::filesystem::path& inputDirOrIndexFile, const std::string& regex) const {

    // If file_to_index.txt exists, pull the file paths from there.
    const std::unordered_set<std::string> exts{ ".txt", ".diff", ".lst" };
    if (exts.contains(inputDirOrIndexFile.extension())) {
        std::vector<fs::path> paths;
        std::ifstream f(inputDirOrIndexFile);

        if (f.is_open()) {
            const auto& directory{inputDirOrIndexFile.parent_path()};
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

    return utils::crawlDirectory(inputDirOrIndexFile, regex);
}

/***********************************************************************************/
void TimestampMapper::deleteIndexFile() {
    if (m_indexFileExists) {
        if (!fs::remove(m_cliOptions.FileListPath)) {
            std::cerr << "Failed to delete index file: " << m_cliOptions.FileListPath << '\n';
            return;
        }
        m_indexFileExists = false;
    }
}

} // namespace tsm
