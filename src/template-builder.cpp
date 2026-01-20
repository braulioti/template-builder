// CLI entrypoint for Template Builder.
// Note: reusable logic lives in `template-builder-lib.cpp` so unit tests can link
// it without pulling in this file's `main()`.
#include <iostream>
#include <string>
#include "template-builder.hpp"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif
#include <curl/curl.h>

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
    std::cout << "* TEMPLATE BUILDER - VERSION 0.1.0                *" << std::endl;
    std::cout << "* Generate project templates using YAML files     *" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << std::endl;
    auto* info = curl_version_info(CURLVERSION_NOW);
    std::cout << "SSL: " << (info->ssl_version ? info->ssl_version : "NONE") << std::endl;

    // Check if YAML file path was provided
    if (!validateArguments(argc)) {
        showUsage(argv[0]);
        return 1;
    }

    std::string yamlFilePath = argv[1];

    // Check if file exists
    if (!fileExists(yamlFilePath)) {
        std::cerr << "Error: File not found: " << yamlFilePath << std::endl;
        return 1;
    }

    return processYamlFile(yamlFilePath);
}
