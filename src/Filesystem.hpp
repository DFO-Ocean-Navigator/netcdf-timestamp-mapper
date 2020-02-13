#pragma once

// Compatibility wrapper for older versions of GCC (5-7)
// which don't fully implement the filesystem library.

#ifdef __has_include
    #if __has_include(<filesystem>)
        #include <filesystem>

        namespace fs = ::std::filesystem;
    #else
        #include <experimental/filesystem>

        namespace fs = ::std::experimental::filesystem;
    #endif
#endif