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
TimestampMapper::TimestampMapper(const std::filesystem::path& inputDir,
                                 const std::filesystem::path& outputDir,
                                 const std::string& datasetName,
                                 const std::string& regexPattern,
                                 const std::string& fileList,
                                 const ds::DATASET_TYPE datasetType,
                                 const bool regenIndices,
                                 const bool dryRun) : 
                                                        m_inputDir{ sanitizeDirectoryPath(inputDir) },
                                                        m_outputDir{ sanitizeDirectoryPath(outputDir) },
                                                        m_datasetName{ datasetName },
                                                        m_regexPattern{ regexPattern },
                                                        m_datasetType{ datasetType },
                                                        m_regenIndices{ regenIndices },
                                                        m_dryRun{ dryRun },
                                                        m_filesToIndexPath{ fileList },
                                                        m_indexFileExists{ fileOrDirExists(m_filesToIndexPath) },
                                                        m_database{ m_inputDir, m_outputDir, m_datasetName }
{
}

/***********************************************************************************/
bool TimestampMapper::exec() {
    if (m_dryRun) {
        std::cout << "---DRY RUN---\n";
    }

    if (!fileOrDirExists(m_inputDir)) {
        std::cerr << "Input directory " << m_inputDir << " does not exist." << std::endl;
        return false;
    }

    if (!createDirectory(m_outputDir)) {
        std::cerr << "Failed to create output directory " << m_outputDir << std::endl;
        return false;
    }

    if (m_indexFileExists) {
        std::cout << "Found list of non-indexed files. Only the files contained in this list will be indexed..." << std::endl;
    }
    else {
        std::cout << "List of non-indexed files not found. Continuing with complete indexing operation..." << std::endl;
    }

    std::cout << "Creating list of all .nc files in " << (m_indexFileExists ? m_filesToIndexPath : m_inputDir) << "..." << std::endl;
    const auto& filePaths{ createFileList(m_indexFileExists ? m_filesToIndexPath : m_inputDir, m_regexPattern) };
    if (filePaths.empty()) {
        std::cout << "No .nc files found." << "\nExiting..." << std::endl;
        return false;
    }

    if (m_dryRun) {
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

    if (m_indexFileExists) {
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
        std::vector<fs::path> paths;

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
        if (!fs::remove(m_filesToIndexPath)) {
            std::cerr << "Failed to delete index file: " << m_filesToIndexPath << '\n';
            return;
        }
        m_indexFileExists = false;
    }
}

} // namespace tsm
