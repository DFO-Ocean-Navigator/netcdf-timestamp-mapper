#include <catch2/single_include/catch2/catch.hpp>
#include "../src/FileReaders/NCFileReader.hpp"

#include <unordered_set>

using namespace tsm;

/***********************************************************************************/
TEST_CASE("1: NCFileDesc returns correct DatafileDesc.") {
    NCFileReader r{ "./Fixtures/giops_forecast.nc" };

    const auto& desc{ r.getDataFileDesc() };

    REQUIRE( desc.NCFilePath == "./Fixtures/giops_forecast.nc" );
    REQUIRE( desc.Timestamps.size() == 1 );
    REQUIRE( desc.Variables.size() == 19 );

    REQUIRE( desc.Timestamps[0] == 2208816000);

    std::unordered_set<std::string> vars(19);
    std::transform(desc.Variables.cbegin(), desc.Variables.cend(), std::inserter(vars, vars.begin()), [](const auto& var) -> std::string {
        return var.Name;
    });

    REQUIRE( vars.count("votemper") > 0);
    REQUIRE( vars.count("longitude") > 0);
    REQUIRE( vars.count("latitude") > 0);
    REQUIRE( vars.count("time") > 0);
    REQUIRE( vars.count("iicevol") > 0);
    REQUIRE( vars.count("isnowvol") > 0);
    REQUIRE( vars.count("iicesurftemp") > 0);
    REQUIRE( vars.count("iiceconc") > 0);
    REQUIRE( vars.count("itzocrtx") > 0);
    REQUIRE( vars.count("itmecrty") > 0);
    REQUIRE( vars.count("iicestrength") > 0);
    REQUIRE( vars.count("iicepressure") > 0);
    REQUIRE( vars.count("vosaline") > 0);
    REQUIRE( vars.count("sossheig") > 0);
    REQUIRE( vars.count("somixhgt") > 0);
    REQUIRE( vars.count("sokaraml") > 0);
    REQUIRE( vars.count("vozocrtx") > 0);
    REQUIRE( vars.count("vomecrty") > 0);

}

/***********************************************************************************/
TEST_CASE("2: Bad file results in empty DatafileDesc.") {
    NCFileReader r{ "" };

    const auto& desc{ r.getDataFileDesc() };

    REQUIRE_FALSE(desc);
}

/***********************************************************************************/
TEST_CASE("3: getDataFileDesc doesn't throw given a bad file.") {
    NCFileReader r{ "" };

    REQUIRE_NOTHROW( r.getDataFileDesc() );
}
