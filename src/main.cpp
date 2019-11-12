#include "TimestampMapper.hpp"

#include "CLIOptions.hpp"

/***********************************************************************************/
int main(int argc, char** argv) {

    std::iostream::sync_with_stdio(false);

    const auto& result{ tsm::cli::parseCmdLineOptions(argc, argv) };
    if (!result) {
        return EXIT_FAILURE;
    }

    if ((*result).count("help")) {
        tsm::cli::printHelp();
        return 0;
    }

    const tsm::cli::CLIOptions opts{ *result };
    if (!opts.verify()) {
        return EXIT_FAILURE;
    }

    tsm::TimestampMapper mapper{opts.InputDir,
                                opts.OutputDir,
                                opts.DatasetName,
                                opts.RegexPattern,
                                opts.FileListPath,
                                opts.Forecast ? tsm::ds::DATASET_TYPE::FORECAST : tsm::ds::DATASET_TYPE::HISTORICAL, 
                                opts.RegenIndices,
                                opts.DryRun
                                };

    return mapper.exec();
}
