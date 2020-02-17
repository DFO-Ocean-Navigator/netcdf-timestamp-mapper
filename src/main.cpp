#include "TimestampMapper.hpp"

#include "CommandLine.hpp"

/***********************************************************************************/
int main(int argc, char** argv) {

    std::iostream::sync_with_stdio(false);

    tsm::CommandLine cli;
    if (!cli.parse(argc, argv)) {
        return EXIT_FAILURE;
    }

    const auto& opts{ cli.getOptions() };

    tsm::TimestampMapper mapper{ opts };

    return mapper.exec();
}
