#include "TimestampMapper.hpp"

#include "DatasetUitls.hpp"

#include <exception>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

namespace tsm {

TimestampMapper::TimestampMapper(const std::filesystem::path& inputDir,
                                 const std::filesystem::path& outputDir,
                                 const std::string& datasetName,
                                 const bool regenIndices) : m_inputDir{ sanitizeDirectoryPath(inputDir) },
                                                            m_outputDir{ sanitizeDirectoryPath(outputDir) },
                                                            m_datasetName{ sanitizeDatasetName(datasetName) },
                                                            m_regenIndices{ regenIndices },
                                                            m_filesToIndexPath{ m_inputDir / "files_to_index.txt" },
                                                            m_indexFileExists{ fileOrDirExists(m_filesToIndexPath) }
{
}

bool TimestampMapper::exec() {
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

    const auto& filePaths{ createFileList(m_indexFileExists ? m_filesToIndexPath : m_inputDir) };
    if (filePaths.empty()) {
        std::cout << "No .nc files found." << "\nExiting..." << std::endl;
        return false;
    }

    const auto& pairs{ ds::createPairs(filePaths) };
    if (!pairs) {
        std::cerr << "Failed to find time dimension in one of the NetCDF files." << std::endl;
        return false;
    }

    return true;
}

bool TimestampMapper::createDirectory(const std::filesystem::path& path) const noexcept {
    std::error_code e;
    fs::create_directory(path, e);

    if (e) {
        std::cerr << e.message() << std::endl;
        return false;
    }
    return true;
}

std::vector<fs::path> TimestampMapper::createFileList(const std::filesystem::path& inputDirOrIndexFile) const {
    std::vector<fs::path> paths;

    if (inputDirOrIndexFile.extension() == ".txt") {
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

    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    const auto options{fs::directory_options::follow_directory_symlink};

    for (const auto& file : recursive_directory_iterator(inputDirOrIndexFile, options)) {
        if (fs::path(file).extension() == ".nc") {
            paths.emplace_back(file);
        }
    }

    return paths;
}

} // namespace tsm
