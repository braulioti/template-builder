#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

void showUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <arquivo.yaml>" << std::endl;
}

void printVariables(const YAML::Node& variablesNode) {
    if (!variablesNode.IsDefined() || !variablesNode.IsSequence()) {
        std::cout << "No variables found or variables is not a sequence." << std::endl;
        return;
    }

    std::cout << "Variables:" << std::endl;
    std::cout << "----------" << std::endl;

    for (const auto& variable : variablesNode) {
        if (!variable.IsMap()) {
            continue;
        }

        std::string name;
        std::string type;

        if (variable["name"].IsDefined()) {
            name = variable["name"].as<std::string>();
        }

        if (variable["type"].IsDefined()) {
            type = variable["type"].as<std::string>();
        }

        if (!name.empty() && !type.empty()) {
            std::cout << "Name: " << name << " | Type: " << type << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "* TEMPLATE BUILDER - VERSION 0.1.0                *" << std::endl;
    std::cout << "* Generate project templates using YAML files     *" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << std::endl;

    // Check if YAML file path was provided
    if (argc < 2) {
        showUsage(argv[0]);
        return 1;
    }

    std::string yamlFilePath = argv[1];

    // Check if file exists
    if (!std::filesystem::exists(yamlFilePath)) {
        std::cerr << "Error: File not found: " << yamlFilePath << std::endl;
        return 1;
    }

    try {
        // Load YAML file
        YAML::Node config = YAML::LoadFile(yamlFilePath);

        // Check if variables section exists
        if (!config["variables"].IsDefined()) {
            std::cout << "No 'variables' section found in YAML file." << std::endl;
            return 0;
        }

        // Print variables
        printVariables(config["variables"]);

        std::cout << std::endl;
        std::cout << "Template Builder execution completed successfully." << std::endl;
        std::cout << std::endl;

    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
