#include "TimestampMapper.hpp"

#include "DatasetDesc.hpp"
#include "CrawlDirectory.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <regex>

namespace tsm {

/***********************************************************************************/
TimestampMapper::TimestampMapper(const CommandLine::Options& opts) : m_datasetType{ opts.Forecast ? tsm::ds::DATASET_TYPE::FORECAST : tsm::ds::DATASET_TYPE::HISTORICAL },
                                                        m_cliOptions{ opts },
                                                        m_indexFileExists{ fileOrDirExists(opts.FileListPath) },
                                                        m_database{ opts.OutputDir, opts.DatasetName }
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

    const auto& filePaths{ createFileList() };
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
bool TimestampMapper::createDirectory(const fs::path& path) const noexcept {
    std::error_code e;
    fs::create_directory(path, e);

    if (e) {
        std::cerr << e.message() << std::endl;
        return false;
    }
    return true;
}

/***********************************************************************************/
std::vector<fs::path> TimestampMapper::createFileList() const {
    std::vector<fs::path> paths;
    
    if (!m_cliOptions.STDINfileList.empty()) {
        std::cout << "Using STDIN for input file list..." << std::endl;

        paths.reserve(m_cliOptions.STDINfileList.size());
        std::transform(m_cliOptions.STDINfileList.cbegin(), m_cliOptions.STDINfileList.cend(), paths.begin(), [](const auto& p) {
            return fs::path(p);
        });

        return paths;
    }


    if (m_indexFileExists) {
        std::cout << "Found list of non-indexed files. Only the files contained in this list will be indexed..." << std::endl;

        // If file_to_index.txt exists, pull the file paths from there.
        const std::unordered_set<std::string> exts{ ".txt", ".diff", ".lst" };
        if (exts.count(m_cliOptions.FileListPath.extension()) > 0) {
            std::ifstream f(m_cliOptions.FileListPath);

            if (f.is_open()) {
                const auto& directory{m_cliOptions.FileListPath.parent_path()};
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
    }

    std::cout << "Continuing with complete indexing operation..." << std::endl;

    return utils::crawlDirectory(m_cliOptions.InputDir, m_cliOptions.RegexPattern, m_cliOptions.RegexEngine);
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
