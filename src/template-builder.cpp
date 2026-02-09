// CLI entrypoint for Template Builder.
// Note: reusable logic lives in `template-builder-lib.cpp` so unit tests can link
// it without pulling in this file's `main()`.
#include <iostream>
#include <string>
#include <cstdlib>
#include "template-builder.hpp"
#include "config/Config.hpp"
#include "config/ConfigConstants.hpp"
#include "services/ArgumentParserService.hpp"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

namespace {

void setupConsoleUtf8() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
#endif
}

void printBanner() {
    std::cout << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "* TEMPLATE BUILDER - VERSION 0.2.0                *" << std::endl;
    std::cout << "* Generate project templates using YAML files     *" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << std::endl;
}

bool handleHelpOrInvalid(TemplateBuilder::ArgumentParserService& parser, const TemplateBuilder::ArgumentParserResult& result, const char* programName) {
    if (!result.showHelp && result.valid) return false;
    parser.showUsage(programName);
    return true;
}

void setInteractiveEnvIfNeeded(bool forceInteractive) {
    if (!forceInteractive) return;
#ifdef _WIN32
    _putenv_s("TEMPLATE_BUILDER_INTERACTIVE", "1");
#else
    setenv("TEMPLATE_BUILDER_INTERACTIVE", "1", 1);
#endif
}

int runYamlOrReportError(const std::string& yamlFilePath) {
    if (!fileExists(yamlFilePath)) {
        std::cerr << "Error: File not found: " << yamlFilePath << std::endl;
        return 1;
    }
    return processYamlFile(yamlFilePath);
}

} // namespace

int main(int argc, char* argv[]) {
    setupConsoleUtf8();
    printBanner();

    TemplateBuilder::Config config;
    std::string listUrl = config.get(
        TemplateBuilder::ConfigConstants::SECTION_ARGUMENTS,
        TemplateBuilder::ConfigConstants::KEY_LIST_URL,
        TemplateBuilder::ConfigConstants::DEFAULT_LIST_URL);
    TemplateBuilder::ArgumentParserService parser(listUrl);
    auto result = parser.parse(argc, argv);

    if (handleHelpOrInvalid(parser, result, argv[0])) {
        return result.showHelp ? 0 : 1;
    }
    if (result.showList) {
        return parser.fetchAndPrintList() ? 0 : 1;
    }

    setInteractiveEnvIfNeeded(result.forceInteractive);
    return runYamlOrReportError(result.getYamlFilePath());
}
