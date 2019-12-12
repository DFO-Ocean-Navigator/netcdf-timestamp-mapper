#pragma once

#include "FileReader.hpp"

#include "../VariableDesc.hpp"

#include <filesystem>
#include <iostream>

#include <ncFile.h>

namespace tsm {

class NCFileReader : public FileReader<NCFileReader> {

public:
    ///
    explicit NCFileReader(const std::filesystem::path& path);
    ///
    ~NCFileReader() {
        m_file.close();
    }

    ///
    [[nodiscard]] ds::DataFileDesc getDataFileDesc_impl();

private:
    ///
    [[nodiscard]] bool open_file();
    ///
    [[nodiscard]] std::string findTimeDim() const;
    ///
    [[nodiscard]] std::vector<ds::VariableDesc> getNCFileVariables() const;
    ///
    [[nodiscard]] std::vector<ds::timestamp_t> getTimestampValues() const;

    const std::filesystem::path m_path;
    netCDF::NcFile m_file;
};

} // namespace tsm
