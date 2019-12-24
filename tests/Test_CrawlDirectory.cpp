#include <catch2/single_include/catch2/catch.hpp>
#include "../src/CrawlDirectory.hpp"

using namespace tsm::utils;

TEST_CASE( "1. Crawl returns expected files with wildcard regex against all supported engines." ) {

    const std::array<std::string, 6> engines{ "egrep", "basic", "extended", "grep", "awk", "ecmascript" };

    for (const auto& engine : engines) {
        const auto& files{ crawlDirectory(".", ".*", engine) };

        REQUIRE(files.size() == 1);
        REQUIRE(*files.begin() == "./Fixtures/giops_forecast.nc");
    }

}
