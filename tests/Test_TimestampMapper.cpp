#include <catch2/single_include/catch2/catch.hpp>
#include "../src/TimestampMapper.hpp"

std::string random_string(const std::size_t length ) {
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const std::size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;

}

TEST_CASE("1. TimestampMapper runs successfully on correct inputs.") {
    tsm::cli::CLIOptions opts;
    opts.InputDir = "./Fixtures/";
    opts.DatasetName = random_string(6);
    opts.OutputDir = "./";
    opts.Historical = true;

    tsm::TimestampMapper tsm{ opts };

    REQUIRE( tsm.exec() );
}
