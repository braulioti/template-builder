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
    validateFile(file);
    std::filesystem::path filePath = prepareFilePath(file);
    ensureDirectoryExists(filePath);
    std::string fileContent = getFileContent(file);
    writeFile(filePath, fileContent);
}

void FileBuilder::validateFile(const FileData* file) const {
    if (!file) {
        return;
    }

    if (file->getPath().empty()) {
        throw std::runtime_error("File path cannot be empty.");
    }
}

std::filesystem::path FileBuilder::prepareFilePath(const FileData* file) const {
    std::filesystem::path currentDir = std::filesystem::current_path();
    return currentDir / file->getPath();
}

void FileBuilder::ensureDirectoryExists(const std::filesystem::path& filePath) const {
    std::filesystem::path directory = filePath.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
}

std::vector<Variable*> FileBuilder::getVariableList(const FileData* file) const {
    const std::vector<Variable*>* variables = file->getVariables();
    if (variables) {
        return *variables;
    }
    return std::vector<Variable*>();
}

std::string FileBuilder::getFileContent(const FileData* file) const {
    std::vector<Variable*> varList = getVariableList(file);
    
    if (!file->hasPrompt()) {
        // Use static content with variable substitution
        return m_promptBuilder->getContent(file->getContent(), varList);
    } else {
        // Use prompt to get content
        return m_promptBuilder->build(file->getPrompt(), varList);
    }
}

void FileBuilder::writeFile(const std::filesystem::path& filePath, const std::string& content) const {
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
    outFile << content;
    
    if (!outFile.good()) {
        outFile.close();
        throw std::runtime_error("Failed to write file: " + filePath.string());
    }

    outFile.close();
}

} // namespace TemplateBuilder
