#pragma once

#include <memory>
#include <functional>

namespace tsm::utils {

/// Template wrapper around a unique_ptr to allow passing a custom deleter lambda.
template <typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)> >;


} // namespace tsm::utils
