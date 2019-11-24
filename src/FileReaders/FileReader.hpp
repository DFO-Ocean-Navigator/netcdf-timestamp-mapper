#pragma once

#include <vector>

#include "../DataFileDesc.hpp"

// Use the Curiously Recurring Template Pattern
// as a compile-time replacement for standard
// inheritance. CRTP is much faster since all
// "polumorphism" is resolved during compile
// time and not run time.
// https://stackoverflow.com/a/26718782/2231969


namespace tsm {

template<class ReaderSubclass>
class FileReader {

public:
    FileReader() = default;
    ~FileReader() = default;

    [[nodiscard]] ds::DataFileDesc getDataFileDesc() {
        return static_cast<ReaderSubclass*>(this)->getDataFileDesc_impl();
    }

};

} // namespace tsm
