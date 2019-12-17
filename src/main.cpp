#include "TimestampMapper.hpp"

#include "CLIOptions.hpp"

/***********************************************************************************/
int main(int argc, char** argv) {

    std::iostream::sync_with_stdio(false);

    auto result{ tsm::cli::parseCmdLineOptions(argc, argv) };
    if (!result) {
        return EXIT_FAILURE;
    }

    const tsm::cli::CLIOptions opts{ *result };
    if (!opts.verify()) {
        return EXIT_FAILURE;
    }

    tsm::TimestampMapper mapper{ opts };

    return mapper.exec();
}
