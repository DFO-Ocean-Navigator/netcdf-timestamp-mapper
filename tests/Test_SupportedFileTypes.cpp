#include <catch2/single_include/catch2/catch.hpp>
#include "../src/FileReaders/SupportedFileTypes.hpp"

using namespace tsm;

TEST_CASE( "1. supportedFileType returns true for .nc file extensions." ) {
    REQUIRE(supportedFileType(".nc"));
}
