#pragma once

#include <ncFile.h>
#include <ncDim.h>
#include <ncVar.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <optional>
#include <filesystem>

namespace tsm {
namespace ds {

using timestamp_t = unsigned long long;

///
struct [[nodiscard]] TimestampFilenamePair {
    TimestampFilenamePair(const std::vector<std::string>& timestamps, const std::filesystem::path& path) :  Timestamps{timestamps},
                                                                                                            NCFilePath{path} {}
    TimestampFilenamePair(const TimestampFilenamePair&) = default;
    TimestampFilenamePair(TimestampFilenamePair&&) = default;

    TimestampFilenamePair& operator=(const TimestampFilenamePair&) = default;
    TimestampFilenamePair& operator=(TimestampFilenamePair&&) = default;

    std::vector<std::string> Timestamps;
    std::filesystem::path NCFilePath;
};

///
[[nodiscard]] std::optional<std::string> findTimeDim(const netCDF::NcFile& ncFile) {
    for (const auto& dim : ncFile.getDims()) {
        if (dim.first.find("time") != std::string::npos) {
            return std::make_optional(dim.first);
        }
    }

    return std::nullopt;
}

///
[[nodiscard]] auto convertNCTimeToISOExtended(const timestamp_t timestamp) {
    namespace bg = boost::gregorian;
    namespace bp = boost::posix_time;

    const bg::date epoch{ 1950, 1, 1 };
    const bp::ptime timeSinceEpoch(epoch, bp::seconds(timestamp));

    return bp::to_iso_extended_string(timeSinceEpoch);
}

///
[[nodiscard]] std::optional<std::vector<timestamp_t>> getTimestampValues(const std::filesystem::path& path) {

    // Open NC file
    netCDF::NcFile f{path, netCDF::NcFile::read};

    auto timeDim{ findTimeDim(f) };
    if (!timeDim) {
        return std::nullopt;
    }

    // Get timestamp values
    netCDF::NcDim dim;
    netCDF::NcVar var;
    f.getCoordVar(*timeDim, dim, var);

    std::vector<timestamp_t> vals(dim.getSize());
    var.getVar(vals.data());

    // NcFile destructor will close the file and release resources

    return std::make_optional(vals);
}

///
[[nodiscard]] std::optional<std::vector<TimestampFilenamePair>> createPairs(const std::vector<std::filesystem::path>& paths) {

    std::vector<TimestampFilenamePair> pairs;
    pairs.reserve(paths.size());

    for (const auto& path : paths) {
       
        const auto timestamps{ getTimestampValues(path) };
        if (!timestamps) {
            return std::nullopt;
        }
        const auto& vals{ timestamps.value() }; // Get underlyng vector

        // Allocate memory for iso timestamps
        std::vector<std::string> isoTimestamps(vals.size());

        // Convert raw timestamps to ISO Extended.
        std::transform(vals.cbegin(), vals.cend(), isoTimestamps.begin(), convertNCTimeToISOExtended);

        pairs.emplace_back(isoTimestamps, path);
    }
   
    return std::make_optional(pairs);
}

} // namespace ds
} // namespace tsm
