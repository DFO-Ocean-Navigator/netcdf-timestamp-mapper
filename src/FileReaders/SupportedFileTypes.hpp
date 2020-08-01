#pragma once

#include <unordered_set>

namespace tsm {

/***********************************************************************************/
static inline auto supportedFileType(const std::string& fileExtension) {
    const static std::unordered_set<std::string> extensions {
        ".nc"
    };

    return extensions.count(fileExtension) > 0;
}

} // namespace tsm
