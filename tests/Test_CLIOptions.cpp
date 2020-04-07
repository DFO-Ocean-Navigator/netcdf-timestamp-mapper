#include <catch2/single_include/catch2/catch.hpp>
#include "../src/CLIOptions.hpp"

TEST_CASE("1. CLIOptions::verify returns true on valid options.") {
    tsm::cli::CLIOptions opts;
    opts.InputDir = "./Fixtures/";
    opts.DatasetName = "my-dataset";
    opts.OutputDir = "./";
    opts.Historical = true;

    REQUIRE( opts.verify() );
}

TEST_CASE("2. CLIOptions::verify returns false on invalid options.") {
    tsm::cli::CLIOptions opts;
    opts.InputDir = "./Fixtures/";
    opts.DatasetName = "";
    opts.OutputDir = "./";
    opts.Historical = true;

    REQUIRE_FALSE( opts.verify() );

    opts.DatasetName = "asdf";
    opts.Forecast = true;
    REQUIRE_FALSE( opts.verify() );

    opts.Forecast = false;
    opts.Historical = false;
    REQUIRE_FALSE( opts.verify() );

    opts.Historical = true;
    opts.InputDir = "/fake_folder/";
    REQUIRE_FALSE( opts.verify() );

    opts.InputDir = "";
    REQUIRE_FALSE( opts.verify() );

    opts.InputDir = "./Fixtures/";
    opts.OutputDir = "";
    REQUIRE_FALSE( opts.verify() );

    opts.OutputDir = "./";
    opts.RegexEngine = "fake_regex";
    REQUIRE_FALSE( opts.verify() );
}