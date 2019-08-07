#include "TimestampMapper.hpp"

#include "CLIOptions.hpp"

/***********************************************************************************/
int main(int argc, char** argv) {
    using namespace tsm::cli;

    std::iostream::sync_with_stdio(false);

    const auto& result{ parseCmdLineOptions(argc, argv) };
    if (!result) {
        return EXIT_FAILURE;
    }

    if ((*result).count("help")) {
        printHelp();
        return 0;
    }

    const CLIOptions opts{ *result };

    if (opts.Forecast == opts.Historical) {
        std::cerr << "ONE of -forecast OR -historical is required." << std::endl;
        return EXIT_FAILURE;
    }

    tsm::TimestampMapper mapper{opts.InputDir,
                                opts.OutputDir,
                                opts.DatasetName,
                                opts.Forecast ? tsm::ds::DATASET_TYPE::FORECAST : tsm::ds::DATASET_TYPE::HISTORICAL, 
                                opts.RegenIndices
                                };

    return mapper.exec();
}
