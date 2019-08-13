#pragma once

#include <string>
#include <vector>

namespace tsm::ds {

struct [[nodiscard]] VariableDesc {
    VariableDesc(const std::string& name,
                const std::string& units,
                const std::string& longName,
                const float min,
                const float max,
                const std::vector<std::string>& dims) :  Name{name},
                                                         Units{units},
                                                         LongName{longName},
                                                         ValidMin{min},
                                                         ValidMax{max},
                                                         Dimensions{dims} {}

    inline auto operator==(const VariableDesc& rhs) const noexcept {
        return Name == rhs.Name;
    }

    std::string Name;
    std::string Units;
    std::string LongName;
    float ValidMin{ 0.0f };
    float ValidMax{ 0.0f };
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
