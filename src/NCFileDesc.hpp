#pragma once

#include "TypeTimestamp.hpp"

#include <vector>
#include <filesystem>

namespace tsm::ds {

struct [[nodiscard]] NCFileDesc {
    NCFileDesc(const std::vector<timestamp_t>& timestamps, const std::vector<std::string>& variables, const std::filesystem::path& path) :  Timestamps{timestamps},
                                                                                                                                            Variables{variables},
                                                                                                                                            NCFilePath{path} {}

    NCFileDesc(const NCFileDesc&) = default;
    NCFileDesc(NCFileDesc&&) = default;

    NCFileDesc& operator=(const NCFileDesc&) = default;
    NCFileDesc& operator=(NCFileDesc&&) = default;

    inline auto operator!() const {
        return Timestamps.empty();
    }

    const std::vector<timestamp_t> Timestamps;
    const std::vector<std::string> Variables;
    const std::filesystem::path NCFilePath;
};

} // namespace tsm
