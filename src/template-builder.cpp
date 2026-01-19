#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "template-builder.hpp"

void showUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <arquivo.yaml>" << std::endl;
}

bool validateArguments(int argc) {
    return argc >= 2;
}

bool fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

int processYamlFile(const std::string& yamlFilePath) {
    try {
        // Load YAML file
        YAML::Node config = YAML::LoadFile(yamlFilePath);

        // Basic validation - file was loaded successfully
        if (!config.IsDefined()) {
            std::cerr << "Error: Failed to load YAML file." << std::endl;
            return 1;
        }

        std::cout << std::endl;
        std::cout << "Template Builder execution completed successfully." << std::endl;
        std::cout << std::endl;
        return 0;

    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    std::cout << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "* TEMPLATE BUILDER - VERSION 0.2.0                *" << std::endl;
    std::cout << "* Generate project templates using YAML files     *" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << std::endl;

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
