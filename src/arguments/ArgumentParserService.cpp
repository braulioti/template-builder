#include "arguments/ArgumentParserService.hpp"
#include <iostream>

namespace TemplateBuilder {

ArgumentParserResult ArgumentParserService::parse(int argc, char* argv[]) {
    ArgumentParserResult result;
    if (argc < 2) {
        return result;
    }
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            result.showHelp = true;
            result.valid = true;
            return result;
        }
        if (arg == "-i" || arg == "--interactive") {
            result.forceInteractive = true;
        } else if (!arg.empty() && arg[0] != '-') {
            result.yamlFilePath = arg;
            result.valid = true;
            return result;
        }
    }
    return result;
}

void ArgumentParserService::showUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <file.yaml>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --interactive  Force interactive prompts (use when running from an IDE)" << std::endl;
    std::cout << "  -h, --help         Show this help message" << std::endl;
}

} // namespace TemplateBuilder
