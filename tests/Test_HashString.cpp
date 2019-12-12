#include <catch2/single_include/catch2/catch.hpp>
#include "../src/Utils/HashString.hpp"

using namespace tsm::utils;

TEST_CASE("1. Hash macro computes correct hash.") {
    REQUIRE(HASH_STR_CRC32("my_string") == 47936178);
}
