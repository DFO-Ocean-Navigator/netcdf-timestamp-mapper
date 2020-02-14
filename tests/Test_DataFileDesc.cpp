#include <catch2/single_include/catch2/catch.hpp>
#include "../src/DataFileDesc.hpp"

using namespace tsm::ds;

/***********************************************************************************/
TEST_CASE( "1: DataFileDesc is move constructible." ) {
    REQUIRE( std::is_move_constructible_v<DataFileDesc> );
}

/***********************************************************************************/
TEST_CASE( "2: DataFileDesc is invalid when any member variable is empty." ) {
    const DataFileDesc d1{ {}, {}, "" };
    const DataFileDesc d2{ {12345}, {}, "adsf" };
    const DataFileDesc d3{ {12345}, {VariableDesc{ "votemper", "units", "Temp", 0.0f, 0.0f, {} }}, ""};
    const DataFileDesc d4{ {12345}, {VariableDesc{ "votemper", "units", "Temp", 0.0f, 0.0f, {} }}, "23423"};

    REQUIRE( !d1 );
    REQUIRE( !d2 );
    REQUIRE( !d3 );

    REQUIRE_FALSE( !d4 );
    REQUIRE( d4 );
}

/***********************************************************************************/
TEST_CASE( "3: DataFileDesc::operator! is marked noexcept." ) {
    const DataFileDesc d1{ {}, {}, "" };

    REQUIRE( noexcept(d1.operator!) );
}

/***********************************************************************************/
TEST_CASE( "4: DataFileDesc::operator bool is marked noexcept." ) {
    const DataFileDesc d1{ {}, {}, "" };

    REQUIRE( noexcept(d1.operator bool) );
}

/***********************************************************************************/
TEST_CASE( "5: All DataFileDesc members are const-qualified." ) {
    
    REQUIRE( std::is_const_v<decltype(DataFileDesc::Timestamps)> );
    REQUIRE( std::is_const_v<decltype(DataFileDesc::Variables)> );
    REQUIRE( std::is_const_v<decltype(DataFileDesc::NCFilePath)> );
}