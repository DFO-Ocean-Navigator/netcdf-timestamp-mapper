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

/***********************************************************************************/
TEST_CASE( "3: All VariableDesc members are const-qualified." ) {
    
    REQUIRE( std::is_const_v<decltype(VariableDesc::Name)> );
    REQUIRE( std::is_const_v<decltype(VariableDesc::Units)> );
    REQUIRE( std::is_const_v<decltype(VariableDesc::LongName)> );
    REQUIRE( std::is_const_v<decltype(VariableDesc::ValidMin)> );
    REQUIRE( std::is_const_v<decltype(VariableDesc::ValidMax)> );
    REQUIRE( std::is_const_v<decltype(VariableDesc::Dimensions)> );
}

/***********************************************************************************/
TEST_CASE( "4: VariableDesc std::hash specialization." ) {
    const VariableDesc d1{ "votemper", "units", "Temp", 0.0f, 0.0f, {} };

    REQUIRE( std::hash<VariableDesc>()(d1) == 7778989162542863968);
}
