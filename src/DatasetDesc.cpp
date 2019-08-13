#include "DatasetDesc.hpp"

#include "ProgressBar.hpp"

#include <ncDim.h>
#include <ncVar.h>

#include <iostream>
#include <algorithm>
#include <regex>

namespace tsm::ds {

/***********************************************************************************/
DatasetDesc::DatasetDesc(const std::vector<std::filesystem::path>& filePaths, const DATASET_TYPE type) : m_datasetType{ type } {
    m_ncFiles.reserve(filePaths.size());

    tsm::utils::ProgressBar pb{ filePaths.size() };
    for (auto i = 0; i < filePaths.size(); ++i) {
        createAndAppendNCFileDesc(filePaths[i]);
        ++pb;
    }
}

/***********************************************************************************/
std::optional<netCDF::NcFile> DatasetDesc::openNCFile(const std::filesystem::path& path) const {
    try {
        return std::make_optional<netCDF::NcFile>(path, netCDF::NcFile::read);
    }
    catch (const netCDF::exceptions::NcException& e) {
        std::cerr << "NetCDF error in: " << path << std::endl;
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }

    return std::nullopt;
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
std::optional<std::vector<timestamp_t>> DatasetDesc::getTimestampValues(const netCDF::NcFile& ncFile) const {
    try {
        auto timeDim{ findTimeDim(ncFile) };
        if (!timeDim) {
            return std::nullopt;
        }

        // Get timestamp values
        netCDF::NcDim dim;
        netCDF::NcVar var;
        ncFile.getCoordVar(*timeDim, dim, var);

        std::vector<timestamp_t> vals(dim.getSize());
        var.getVar(vals.data());

        return std::make_optional(vals);
    }
    catch (const netCDF::exceptions::NcException& e) {
        std::cerr << "Error in getting time dimention values:" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }

    return std::nullopt;
}

/***********************************************************************************/
std::vector<VariableDesc> DatasetDesc::getNCFileVariables(const netCDF::NcFile& ncFile) const {
    std::vector<VariableDesc> variables;

    const auto varCount{ ncFile.getVarCount() };
    if (varCount < 1) {
        return variables;
    }
    variables.reserve(static_cast<std::size_t>(varCount));

    for (const auto& pair : ncFile.getVars()) {
        
        const auto& atts{ pair.second.getAtts() };

        // Find variable units
        std::string units;
        if (atts.contains("units")) {
            atts.find("units")->second.getValues(units);
        }

        std::string longName{ pair.first };
        if (atts.contains("long_name")) {
            atts.find("long_name")->second.getValues(longName);
        }

        float validMin{ std::numeric_limits<float>::min() };
        if (atts.contains("valid_min")) {
            atts.find("valid_min")->second.getValues(&validMin);
        }

        float validMax{ std::numeric_limits<float>::max() };
        if (atts.contains("valid_max")) {
            atts.find("valid_max")->second.getValues(&validMax);
        }

        // Get names of variable dimensions
        const auto& dims { pair.second.getDims() };
        std::vector<std::string> dimNames(dims.size());
        std::transform(dims.cbegin(), dims.cend(), dimNames.begin(), [](const auto& ncDim) {
            return ncDim.getName();
        });


        variables.emplace_back(pair.first, units, longName, validMin, validMax, dimNames); // Variable names are stored in first value.
    }

    /*
    // Filter out non-data variables (time, lat/lon, etc)
    const static std::regex filterPattern{ "^(.)*(time|depth|lat|lon|polar|^x|^y)+(.)*$" }; // https://regexr.com/4i448
    const auto res = std::remove_if(variableNames.begin(), variableNames.end(), [](const auto& varName) {
        return std::regex_match(varName, filterPattern);
    });
    variableNames.erase(res, variableNames.end()); // Chain erase to actually drop the filtered items from the vector
    */

    // Cool way to print container contents to the console...
    //std::copy(variableNames.cbegin(), variableNames.cend(), std::ostream_iterator<std::string>(std::cout, ", "));

    return variables;
}

/***********************************************************************************/
void DatasetDesc::createAndAppendNCFileDesc(const std::filesystem::path& path) {

    const auto& ncFile{ openNCFile(path) };
    if (!ncFile) {
        return;
    }

    const auto timestamps{ getTimestampValues(*ncFile) };
    if (!timestamps) {
        std::cerr << "Error finding time dimension in " << path << ". This file will NOT be indexed." << std::endl;
        return;
    }
    const auto& vals{ timestamps.value() }; // Get underlyng vector

    const auto& variables{ getNCFileVariables(*ncFile) };

    m_ncFiles.emplace_back(vals, variables, path);

    // NcFile destructor will close the file and release resources
}

} // namespace tsm::ds
