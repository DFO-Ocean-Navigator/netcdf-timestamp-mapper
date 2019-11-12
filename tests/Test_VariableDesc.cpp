#include <catch2/single_include/catch2/catch.hpp>
#include "../src/VariableDesc.hpp"

using namespace tsm::ds;

/***********************************************************************************/
TEST_CASE( "1: operator== compares against VariableDesc::Name." ) {
    const VariableDesc d1{ "votemper", "units", "Temp", 0.0f, 0.0f, {} };
    const VariableDesc d2{ "votemper", "units", "Temp", 0.0f, 0.0f, {} };
    const VariableDesc d3{ "vosaline", "units", "Temp", 0.0f, 0.0f, {} };

    REQUIRE( d1 == d2 );
    REQUIRE_FALSE( d1 == d3);
}

/***********************************************************************************/
TEST_CASE( "2: operator== is marked noexcept" ) {
    const VariableDesc d1{ "votemper", "units", "Temp", 0.0f, 0.0f, {} };
    
    REQUIRE(noexcept(d1.operator==(d1)));
}
