#pragma once

#error GRIB2 reader not implemented.

#include "../Filesystem.hpp"
#include "FileReader.hpp"

namespace tsm {

class Grib2FileReader : public FileReader<Grib2FileReader> {

public:
    explicit Grib2FileReader(const fs::path& path);

    ///
    [[nodiscard]] ds::DataFileDesc getDataFileDesc_impl();

private:
    const fs::path m_path;
};

} // namespace tsm
