#pragma once

#include <string>
#include <vector>

namespace tsm::ds {

struct [[nodiscard]] VariableDesc {
    VariableDesc(const std::string& name, const std::string units, const std::vector<std::string>& dims) :  Name{name},
                                                                                                            Units{units},
                                                                                                            Dimensions{dims} {}

    inline auto operator==(const VariableDesc& rhs) const noexcept {
        return Name == rhs.Name;
    }

    std::string Name;
    std::string Units;
    std::vector<std::string> Dimensions;
};

} // namespace tsm::dds

namespace std {
  template <>
  struct hash<tsm::ds::VariableDesc> {
    size_t operator()(const tsm::ds::VariableDesc& vd) const {
      return hash<std::string>()(vd.Name);
    };
  };
}
