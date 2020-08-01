#pragma once

#include "Utils/HashString.hpp"
#include "Filesystem.hpp"

#include <regex>
#include <vector>
#include <iostream>

namespace tsm::utils {

///
static inline auto crawlDirectory(const fs::path& inputDirOrIndexFile, const std::string& regex, const std::string& engine) {
    using recursive_dir_iterator = fs::recursive_directory_iterator;

    std::vector<fs::path> paths;
    const auto options{ fs::directory_options::follow_directory_symlink };

    std::regex::flag_type engineType;
    switch (HASH_STR_CRC32(engine.c_str())) {
        case HASH_STR_CRC32("egrep"):
            engineType = std::regex::egrep;
        break;

        case HASH_STR_CRC32("basic"):
            engineType = std::regex::basic;
        break;

        case HASH_STR_CRC32("extended"):
            engineType = std::regex::extended;
        break;

        case HASH_STR_CRC32("grep"):
            engineType = std::regex::grep;
        break;

        case HASH_STR_CRC32("awk"):
            engineType = std::regex::awk;
        break;

        case HASH_STR_CRC32("ecmascript"):
            engineType = std::regex::ECMAScript;
        break;

        default:
            engineType = std::regex::egrep;
        break;
    }

    try {
        const std::regex r(regex, std::regex::optimize | engineType);

        for (const auto& file : recursive_dir_iterator(inputDirOrIndexFile, options)) {
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
