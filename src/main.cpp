#include "TimestampMapper.hpp"

#include <cxxopts/include/cxxopts.hpp>

#include <iostream>
#include <optional>

[[nodiscard]] std::optional<cxxopts::ParseResult> parseCmdLineOptions(int argc, char** argv) {

    try {
        cxxopts::Options options("NetCDF DB Mapper", "One line description of MyProgram");

        options.add_options()
        ("d,input-dir", "Input directory.", cxxopts::value<std::string>())
        ("n,dataset-name", "Dataset name (no spaces).", cxxopts::value<std::string>())
        ("o,output-dir", "Output directory.", cxxopts::value<std::string>())
        ("regen-indices", "Regenerate indices", cxxopts::value<bool>()->implicit_value("false"))
        ;

        const auto& result{ options.parse(argc, argv) };

        return std::make_optional(result);

    } catch(const cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    } catch(...) {
        std::cerr << "Unhandled execption." << std::endl;
        return std::nullopt;
    }
}

int main(int argc, char** argv) {

    const auto& result{ parseCmdLineOptions(argc, argv) };
    if (!result) {
        return EXIT_FAILURE;
    }

    tsm::TimestampMapper mapper{(*result)["input-dir"].as<std::string>(),
                                (*result)["output-dir"].as<std::string>(),
                                (*result)["dataset-name"].as<std::string>(),
                                (*result)["regen-indices"].as<bool>()
                                };

    return mapper.exec();
}
