#include "arguments/ArgumentParserService.hpp"
#include <iostream>

namespace TemplateBuilder {

namespace {
bool isHelp(const std::string& arg) {
    return arg == "-h" || arg == "--help";
}
bool isInteractive(const std::string& arg) {
    return arg == "-i" || arg == "--interactive";
}
bool isYamlPath(const std::string& arg) {
    return !arg.empty() && arg[0] != '-';
}
} // namespace

ArgumentParserResult ArgumentParserService::parse(int argc, char* argv[]) {
    ArgumentParserResult result;
    if (argc < 2) {
        return result;
    }
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (isHelp(arg)) {
            result.showHelp = true;
            result.valid = true;
            return result;
        }
        if (isInteractive(arg)) {
            result.forceInteractive = true;
        } else if (isYamlPath(arg)) {
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
