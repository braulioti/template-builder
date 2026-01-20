#include "builders/FileBuilder.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace TemplateBuilder {

FileBuilder::FileBuilder()
    : m_promptBuilder(std::make_unique<PromptBuilder>()) {
}

void FileBuilder::build(const FileData* file) {
    if (!file) {
        return;
    }

    if (file->getPath().empty()) {
        throw std::runtime_error("File path cannot be empty.");
    }

    // Get current working directory and combine with file path
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path filePath = currentDir / file->getPath();
    
    // Get directory path and create it if it doesn't exist
    std::filesystem::path directory = filePath.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    // Get file content
    std::string fileContent;
    
    if (!file->hasPrompt()) {
        // Use static content with variable substitution
        const std::vector<Variable*>* variables = file->getVariables();
        std::vector<Variable*> varList;
        if (variables) {
            varList = *variables;
        }
        fileContent = m_promptBuilder->getContent(file->getContent(), varList);
    } else {
        // Use prompt to get content
        const std::vector<Variable*>* variables = file->getVariables();
        std::vector<Variable*> varList;
        if (variables) {
            varList = *variables;
        }
        fileContent = m_promptBuilder->build(file->getPrompt(), varList);
    }

    // Write file with UTF-8 encoding
    // Using binary mode ensures UTF-8 bytes are written exactly as stored in std::string
    // std::string in C++17 stores UTF-8 encoded text as a sequence of bytes
    std::ofstream outFile(filePath, std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath.string());
    }

    // UTF-8 BOM is optional - commented out to match original Pascal behavior
    // Some editors prefer files without BOM for better compatibility
    // outFile << "\xEF\xBB\xBF";  // UTF-8 BOM

    // Write UTF-8 encoded content directly
    // Binary mode ensures no encoding conversion occurs
    outFile << fileContent;
    
    if (!outFile.good()) {
        outFile.close();
        throw std::runtime_error("Failed to write file: " + filePath.string());
    }

    outFile.close();
}

} // namespace TemplateBuilder
