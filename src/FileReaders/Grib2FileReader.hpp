#pragma once

#error GRIB2 reader not implemented.

#include "FileReader.hpp"

#include <filesystem>

namespace tsm {

class Grib2FileReader : public FileReader<Grib2FileReader> {

public:
    explicit Grib2FileReader(const std::filesystem::path& path);

    ///
    [[nodiscard]] ds::DataFileDesc getDataFileDesc_impl();

private:
    const std::filesystem::path m_path;
};

} // namespace tsm
