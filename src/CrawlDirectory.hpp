#pragma once

#include <filesystem>
#include <regex>
#include <vector>
#include <iostream>

namespace tsm::utils {

///
auto crawlDirectory(const std::filesystem::path& inputDirOrIndexFile, const std::string& regex) {
    namespace fs = ::std::filesystem;
    using recursive_dir_iterator = fs::recursive_directory_iterator;

    std::vector<fs::path> paths;
    const auto options{ fs::directory_options::follow_directory_symlink };

    try {
        const std::regex r(regex, std::regex::optimize | std::regex::ECMAScript);

        for (const auto& file : recursive_dir_iterator(inputDirOrIndexFile, options)) {
            std::cerr << file << std::endl;
            if (fs::path(file).extension() == ".nc" && std::regex_match(fs::path(file).string(), r)) {
                paths.emplace_back(file);
            }
        }
    }
    catch(const std::regex_error& e) {
        std::cerr << "Regex error: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch(...) {
        std::cerr << "Caught unknown exception." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return paths;
}

} // namespace tsm::utils
