#include "NCFileReader.hpp"

#include <algorithm>

#include <ncDim.h>
#include <ncVar.h>

namespace tsm {
    
/***********************************************************************************/
NCFileReader::NCFileReader(const fs::path& path) : m_path{path} {}

/***********************************************************************************/
ds::DataFileDesc NCFileReader::getDataFileDesc_impl() {
    if (!open_file()) {
        return ds::DataFileDesc();
    }
    
    const auto& timestamps{ getTimestampValues() };
    if (timestamps.empty()) {
        std::cerr << "Error finding time dimension in " << m_path << ". This file will NOT be indexed." << std::endl;
        return ds::DataFileDesc();
    }

    const auto& variables{ getNCFileVariables() };

    return { timestamps, variables, m_path };
}

/***********************************************************************************/
bool NCFileReader::open_file() {
    // figure out whether to re-throw exception
    try {
        m_file.open(m_path, netCDF::NcFile::read);
        return true;
    }
    catch (const netCDF::exceptions::NcException& e) {
        std::cerr << "NetCDF error in: " << m_path << std::endl;
        std::cerr << e.what() << std::endl;
    } 
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }

    return false;
}

/***********************************************************************************/
std::string NCFileReader::findTimeDim() const {
    for (const auto& dim : m_file.getDims()) {
        if (dim.first.find("time") != std::string::npos) {
            return dim.first;
        }
    }

    return std::string();
}

/***********************************************************************************/
std::vector<ds::VariableDesc> NCFileReader::getNCFileVariables() const {
    std::vector<ds::VariableDesc> variables;

    const auto varCount{ m_file.getVarCount() };
    if (varCount < 1) {
        return variables;
    }
    variables.reserve(static_cast<std::size_t>(varCount));

    for (const auto& pair : m_file.getVars()) {
        
        const auto& atts{ pair.second.getAtts() };

        // Find variable units
        std::string units;
        if (atts.count("units") > 0) {
            atts.find("units")->second.getValues(units);
        }

        std::string longName{ pair.first };
        if (atts.count("long_name") > 0) {
            atts.find("long_name")->second.getValues(longName);
        }

        float validMin{ std::numeric_limits<float>::min() };
        if (atts.count("valid_min") > 0) {
            atts.find("valid_min")->second.getValues(&validMin);
        }

        float validMax{ std::numeric_limits<float>::max() };
        if (atts.count("valid_max") > 0) {
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

    return variables;
}

/***********************************************************************************/
std::vector<ds::timestamp_t> NCFileReader::getTimestampValues() const {
   
    try {
        auto timeDim{ findTimeDim() };
        if (timeDim.empty()) {
            return {};
        }

        // Get timestamp values
        netCDF::NcDim dim;
        netCDF::NcVar var;
        m_file.getCoordVar(timeDim, dim, var);

        ds::timestamp_t* v{ (ds::timestamp_t*)calloc(sizeof(ds::timestamp_t), dim.getSize()) };

        var.getVar(v);

        std::vector<ds::timestamp_t> vals(v, v + dim.getSize());

        free(v);

        return vals;
    }
    catch (const netCDF::exceptions::NcException& e) {
        std::cerr << "Error in getting time dimension values:" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
    }

    return {};
}

} // namespace tsm
