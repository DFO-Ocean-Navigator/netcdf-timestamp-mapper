#pragma once

#include "TypeTimestamp.hpp"

#include <vector>
#include <filesystem>

namespace tsm::ds {

struct [[nodiscard]] NCFileDesc {
    NCFileDesc(const std::vector<timestamp_t>& timestamps, const std::filesystem::path& path) :  Timestamps{timestamps},
                                                                                                 NCFilePath{path} {}

    NCFileDesc(const std::vector<timestamp_t>& timestamps, std::vector<unsigned int>& variableIndices, const std::filesystem::path& path) :  Timestamps{timestamps},
                                                                                                                                             Variables{variableIndices},
                                                                                                                                             NCFilePath{path} {}

    NCFileDesc(const NCFileDesc&) = default;
    NCFileDesc(NCFileDesc&&) = default;

    NCFileDesc& operator=(const NCFileDesc&) = default;
    NCFileDesc& operator=(NCFileDesc&&) = default;

    inline auto operator!() const {
        return Timestamps.empty();
    }

    const std::vector<timestamp_t> Timestamps;
    const std::vector<unsigned int> Variables;
    const std::filesystem::path NCFilePath;
};

} // namespace tsm
