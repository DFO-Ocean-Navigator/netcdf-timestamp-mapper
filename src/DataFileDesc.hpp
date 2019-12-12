#pragma once

#include "TypeTimestamp.hpp"

#include "VariableDesc.hpp"

#include <filesystem>

namespace tsm::ds {

struct [[nodiscard]] DataFileDesc {
    ///
    DataFileDesc() noexcept = default;
    ///
    DataFileDesc(const std::vector<timestamp_t>& timestamps, const std::vector<VariableDesc>& variables, const std::filesystem::path& path) :  Timestamps{timestamps},
                                                                                                                                            Variables{variables},
                                                                                                                                            NCFilePath{path} {}

    DataFileDesc(const DataFileDesc&) = default;
    DataFileDesc(DataFileDesc&&) = default;

    DataFileDesc& operator=(const DataFileDesc&) = default;
    DataFileDesc& operator=(DataFileDesc&&) = default;

    explicit operator bool() const noexcept {
        return (!Timestamps.empty()) && (!Variables.empty()) && (!NCFilePath.empty());
    }

    inline auto operator!() const noexcept {
        return !operator bool();
    }

    const std::vector<timestamp_t> Timestamps;
    const std::vector<VariableDesc> Variables;
    const std::filesystem::path NCFilePath;
};

} // namespace tsm
