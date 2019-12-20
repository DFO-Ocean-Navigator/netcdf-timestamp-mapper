#include <catch2/single_include/catch2/catch.hpp>
#include "../src/TypeTimestamp.hpp"

using namespace tsm::ds;

TEST_CASE( "1. timestamp_t is aliased to unsigned long long" ) {
    REQUIRE( typeid(timestamp_t) == typeid(unsigned long long) );
}
