#include <catch2/single_include/catch2/catch.hpp>
#include "../src/FileReaders/NCFileReader.hpp"

#include <unordered_set>

using namespace tsm;

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

    REQUIRE( vars.contains("votemper") );
    REQUIRE( vars.contains("longitude") );
    REQUIRE( vars.contains("latitude") );
    REQUIRE( vars.contains("time") );
    REQUIRE( vars.contains("iicevol") );
    REQUIRE( vars.contains("isnowvol") );
    REQUIRE( vars.contains("iicesurftemp") );
    REQUIRE( vars.contains("iiceconc") );
    REQUIRE( vars.contains("itzocrtx") );
    REQUIRE( vars.contains("itmecrty") );
    REQUIRE( vars.contains("iicestrength") );
    REQUIRE( vars.contains("iicepressure") );
    REQUIRE( vars.contains("vosaline") );
    REQUIRE( vars.contains("sossheig") );
    REQUIRE( vars.contains("somixhgt") );
    REQUIRE( vars.contains("sokaraml") );
    REQUIRE( vars.contains("vozocrtx") );
    REQUIRE( vars.contains("vomecrty") );

}

TEST_CASE("2: Bad file results in empty DatafileDesc.") {
    NCFileReader r{ "" };

    const auto& desc{ r.getDataFileDesc() };

    REQUIRE_FALSE(desc);
}

TEST_CASE("3: getDataFileDesc doesn't throw given a bad file.") {
    NCFileReader r{ "" };

    REQUIRE_NOTHROW( r.getDataFileDesc() );
}
