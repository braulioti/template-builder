#include "services/ArgumentParserService.hpp"
#include "services/arguments/ArgumentList.hpp"
#include <iostream>

namespace TemplateBuilder {

namespace {
bool isHelp(const std::string& arg) {
    return arg == "-h" || arg == "--help";
}
bool isList(const std::string& arg) {
    return arg == "-l" || arg == "--list";
}
bool isInteractive(const std::string& arg) {
    return arg == "-i" || arg == "--interactive";
}
bool isYamlPath(const std::string& arg) {
    return !arg.empty() && arg[0] != '-';
}
} // namespace

ArgumentParserService::ArgumentParserService(std::string listUrl) : m_listUrl(std::move(listUrl)) {}

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
        if (isList(arg)) {
            result.showList = true;
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

void ArgumentParserService::showUsage(const char* programName) const {
    std::cout << "Usage: " << programName << " [options] <file.yaml>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --interactive  Force interactive prompts (use when running from an IDE)" << std::endl;
    std::cout << "  -l, --list         Show templates list from repository" << std::endl;
    std::cout << "  -h, --help         Show this help message" << std::endl;
}

bool ArgumentParserService::fetchAndPrintList() const {
    return ArgumentList::fetchAndPrintList(m_listUrl);
}

} // namespace TemplateBuilder
