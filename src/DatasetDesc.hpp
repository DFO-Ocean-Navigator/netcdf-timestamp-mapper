#pragma once

#include "NCFileDesc.hpp"

#include <ncFile.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <set>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>

namespace tsm {
class Database;
}

namespace tsm::ds {

enum DATASET_TYPE {
    HISTORICAL = 0,
    FORECAST
};

enum VARIABLE_LAYOUT {
    COMBINED = 0,
    SPLIT
};

class DatasetDesc {

friend class ::tsm::Database;

public:
    explicit DatasetDesc(const std::vector<std::filesystem::path>& filePaths, const DATASET_TYPE type, const VARIABLE_LAYOUT layout);

    inline auto operator!() const {
        return m_ncFiles.empty();
    }

private:
    ///
    [[nodiscard]] inline std::optional<std::string> findTimeDim(const netCDF::NcFile& ncFile) const;
    ///
    [[nodiscard]] std::optional<std::vector<timestamp_t>> getTimestampValues(const std::filesystem::path& path) const;
    ///
    [[nodiscard]] inline auto convertNCTimeToISOExtended(const timestamp_t timestamp) {
        namespace bg = boost::gregorian;
        namespace bp = boost::posix_time;

        const bg::date epoch{ 1950, 1, 1 };
        const bp::ptime timeSinceEpoch(epoch, bp::seconds(timestamp));

        return bp::to_iso_extended_string(timeSinceEpoch);
    }
    ///
    void createAndAppendNCFileDesc(const std::filesystem::path& path);

    std::vector<NCFileDesc> m_ncFiles;
    std::set<std::string> m_variables;

    const DATASET_TYPE m_datasetType;
    const VARIABLE_LAYOUT m_varLayout;
};

} // namespace tsm::ds
