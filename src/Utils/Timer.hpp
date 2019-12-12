#pragma once

#include <type_traits>
#include <utility>
#include <chrono>

namespace tsm::utils {

/// https://stackoverflow.com/a/24469673/2231969
template <class Func, class... Args>
inline auto timer(Func func, Args&& ... args) -> typename std::enable_if_t<
    !std::is_same_v<decltype( func( std::forward<Args>(args)... ) ),
    void>,
    std::pair<double, decltype( func( std::forward<Args>(args)... ) ) >
    > {

    static_assert(!std::is_void_v<decltype( func(args...) )>,
                  "Call timer_void if return type is void!");

    const auto& start{ std::chrono::high_resolution_clock::now() };
    auto funcRetVal{ func(args...) };
    const auto& end{ std::chrono::high_resolution_clock::now() };

    const std::chrono::duration<double, std::milli> elapsed{ end - start };

    return { elapsed.count(), funcRetVal };
}

template <class Func, class... Args>
inline auto timer(Func func, Args&& ... args) -> typename std::enable_if_t<
    std::is_same_v<decltype( func( std::forward<Args>(args)... ) ),
    void>
    > {

    static_assert(std::is_void_v<decltype( func(args...) )>,
                  "Call timer for non void return type");

    const auto& start{ std::chrono::high_resolution_clock::now() };
    func(args...);
    const auto& end { std::chrono::high_resolution_clock::now() };

    const std::chrono::duration<double, std::milli> elapsed{ end - start };

    return elapsed.count();
}

} // namespace tsm::utils
