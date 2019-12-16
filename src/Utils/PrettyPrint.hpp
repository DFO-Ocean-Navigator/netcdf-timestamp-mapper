#pragma once

#include <ostream>

namespace tsm::utils {

std::ostream& bold_on(std::ostream& os) {
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os) {
    return os << "\e[0m";
}

} //namespace tsm::utils
