// Shared implementation for Template Builder.
// This file intentionally does NOT contain `main()` so it can be linked from unit tests.

#include "template-builder.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include <yaml-cpp/yaml.h>
#include "services/ParseYAML.hpp"

void showUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <file.yaml>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --interactive  Force interactive prompts (use when running from an IDE)" << std::endl;
}

bool validateArguments(int argc, char* argv[], std::string& outYamlPath, bool& outForceInteractive) {
    outYamlPath.clear();
    outForceInteractive = false;
    if (argc < 2) {
        return false;
    }
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i" || arg == "--interactive") {
            outForceInteractive = true;
        } else if (!arg.empty() && arg[0] != '-') {
            outYamlPath = arg;
            break;
        }
    }
    return !outYamlPath.empty();
}

bool fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

int processYamlFile(const std::string& yamlFilePath) {
    try {
        // Create parser and load YAML file
        TemplateBuilder::ParserYAML parser(yamlFilePath);

        // Build all files, folders, and remote files
        // buildAll handles exceptions internally and continues processing
        parser.buildAll();

        std::cout << std::endl;
        std::cout << "Template successfully generated." << std::endl;
        std::cout << "Thanks for using Template Builder!  :)" << std::endl;
        std::cout << std::endl;
        return 0;

    } catch (const TemplateBuilder::UnsupportedTemplateVersion& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

