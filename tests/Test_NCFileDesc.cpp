#include <catch2/single_include/catch2/catch.hpp>
#include "../src/NCFileDesc.hpp"

using namespace tsm::ds;

/***********************************************************************************/
TEST_CASE( "1: NCFileDesc is move constructible." ) {
    REQUIRE( std::is_move_constructible_v<NCFileDesc> );
}

/***********************************************************************************/
TEST_CASE( "2: NCFileDesc is invalid when any member variable is empty" ) {
    const NCFileDesc d1{ {}, {}, "" };
    const NCFileDesc d2{ {12345}, {}, "adsf" };
    const NCFileDesc d3{ {12345}, {VariableDesc{ "votemper", "units", "Temp", 0.0f, 0.0f, {} }}, ""};
    const NCFileDesc d4{ {12345}, {VariableDesc{ "votemper", "units", "Temp", 0.0f, 0.0f, {} }}, "23423"};

    REQUIRE( !d1 );
    REQUIRE( !d2 );
    REQUIRE( !d3 );

    REQUIRE_FALSE( !d4 );
}

/***********************************************************************************/
TEST_CASE( "3: NCFileDesc::operator! is marked noexcept." ) {
    const NCFileDesc d1{ {}, {}, "" };

    REQUIRE( noexcept(d1.operator!) );
}
