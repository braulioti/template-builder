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

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Set console to UTF-8 mode for proper handling of special characters
    SetConsoleOutputCP(65001);  // UTF-8 code page
    SetConsoleCP(65001);        // UTF-8 code page for input
    // Also set stdin/stdout to binary mode to preserve UTF-8 bytes
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
#endif

    std::cout << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "* TEMPLATE BUILDER - VERSION 0.2.0                *" << std::endl;
    std::cout << "* Generate project templates using YAML files     *" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << std::endl;

    TemplateBuilder::Config config;
    std::string listUrl = config.get(
        TemplateBuilder::ConfigConstants::SECTION_ARGUMENTS,
        TemplateBuilder::ConfigConstants::KEY_LIST_URL,
        TemplateBuilder::ConfigConstants::DEFAULT_LIST_URL);
    TemplateBuilder::ArgumentParserService parser(listUrl);
    auto result = parser.parse(argc, argv);
    if (result.showHelp || !result.valid) {
        parser.showUsage(argv[0]);
        return result.showHelp ? 0 : 1;
    }

    if (result.showList) {
        return parser.fetchAndPrintList() ? 0 : 1;
    }

    if (result.forceInteractive) {
#ifdef _WIN32
        _putenv_s("TEMPLATE_BUILDER_INTERACTIVE", "1");
#else
        setenv("TEMPLATE_BUILDER_INTERACTIVE", "1", 1);
#endif
    }

    // Check if file exists
    const std::string& yamlFilePath = result.getYamlFilePath();
    if (!fileExists(yamlFilePath)) {
        std::cerr << "Error: File not found: " << yamlFilePath << std::endl;
        return 1;
    }

    return processYamlFile(yamlFilePath);
}
