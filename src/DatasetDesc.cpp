#include "DatasetDesc.hpp"

#include <ncDim.h>
#include <ncVar.h>

#include <iostream>

namespace tsm::ds {

/***********************************************************************************/
DatasetDesc::DatasetDesc(const std::vector<std::filesystem::path>& filePaths, const DATASET_TYPE type, const VARIABLE_LAYOUT layout) :
                                                                                                                                        m_datasetType{ type },
                                                                                                                                        m_varLayout{ layout } {
    m_ncFiles.reserve(filePaths.size());

    for (const auto& path : filePaths) {
        createAndAppendNCFileDesc(path);
    }
}

/***********************************************************************************/
std::optional<std::string> DatasetDesc::findTimeDim(const netCDF::NcFile& ncFile) const {
    for (const auto& dim : ncFile.getDims()) {
        if (dim.first.find("time") != std::string::npos) {
            return std::make_optional(dim.first);
        }
    }

    return std::nullopt;
}

/***********************************************************************************/
std::optional<std::vector<timestamp_t>> DatasetDesc::getTimestampValues(const std::filesystem::path& path) const {
    try {
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
    catch (const netCDF::exceptions::NcException& e) {
        std::cerr << "NetCDF error in " << path << std::endl;
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }

    return std::nullopt;
}

/***********************************************************************************/
void DatasetDesc::createAndAppendNCFileDesc(const std::filesystem::path& path) {
    
    const auto timestamps{ getTimestampValues(path) };
    if (!timestamps) {
        std::cerr << "Error finding time dimension in " << path << ". This file will NOT be indexed." << std::endl;
        return;
    }
    
    const auto& vals{ timestamps.value() }; // Get underlyng vector

    m_ncFiles.emplace_back(vals, path);
}

} // namespace tsm::ds
