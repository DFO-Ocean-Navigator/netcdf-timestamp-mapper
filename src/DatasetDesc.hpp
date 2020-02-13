#pragma once

#include "Filesystem.hpp"
#include "DataFileDesc.hpp"

#include <ncFile.h>
//#include <boost/date_time/gregorian/gregorian.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace tsm {
class Database;
}

namespace tsm::ds {

enum DATASET_TYPE {
    HISTORICAL = 0,
    FORECAST
};

class DatasetDesc {

friend class ::tsm::Database;

public:
    ///
    DatasetDesc(const std::vector<fs::path>& filePaths, const DATASET_TYPE type);

    explicit operator bool() const noexcept {
        return !m_ncFiles.empty();
    }

    auto operator!() const noexcept {
        return !operator bool();
    }

    ///
    inline auto isHistorical() const noexcept {
        return m_datasetType == DATASET_TYPE::HISTORICAL;
    }
    ///
    inline auto isForecast() const noexcept {
        return m_datasetType == DATASET_TYPE::FORECAST;
    }

private:
    std::vector<DataFileDesc> m_ncFiles;

    const DATASET_TYPE m_datasetType;
};

} // namespace tsm::ds
