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
    try {
        validateFile(file);
        std::filesystem::path filePath = prepareFilePath(file);
        ensureDirectoryExists(filePath);
        std::string fileContent = getFileContent(file);
        writeFile(filePath, fileContent);
    } catch (const std::exception& e) {
        // Re-throw with more context
        throw std::runtime_error("Error building file " + (file ? file->getPath() : "unknown") + ": " + e.what());
    }
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
    // Get parent directory path (e.g., "assets" from "assets/main.js")
    std::filesystem::path directory = filePath.parent_path();
    // Create directory structure if it doesn't exist (creates all parent directories)
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
    
    try {
        std::string content;
        if (!file->hasPrompt()) {
            // Use static content with variable substitution
            // If content is empty and no prompt, file will be created with a space
            content = m_promptBuilder->getContent(file->getContent(), varList);
        } else {
            // Use prompt to get content
            content = m_promptBuilder->build(file->getPrompt(), varList);
        }
        
        // If content is empty, return a space to ensure file is created with at least one character
        // This prevents issues with empty file creation
        if (content.empty()) {
            return " ";
        }
        
        return content;
    } catch (const std::exception& e) {
        // Re-throw with more context
        throw std::runtime_error("Error processing file content for " + file->getPath() + ": " + e.what());
    }
}

void FileBuilder::writeFile(const std::filesystem::path& filePath, const std::string& content) const {
    // Write file with UTF-8 encoding (without BOM for better compatibility)
    // Using binary mode ensures UTF-8 bytes are written exactly as stored in std::string
    // This preserves special characters (accents, etc.) correctly
    // std::ios::trunc ensures the file is created even if content is empty
    std::ofstream outFile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath.string());
    }

    // Write UTF-8 encoded content directly
    // Binary mode ensures no encoding conversion occurs, preserving UTF-8 bytes exactly
    // Always write content (even if it's just a space for empty files)
    outFile.write(content.c_str(), content.size());
    
    if (!outFile.good()) {
        outFile.close();
        throw std::runtime_error("Failed to write file: " + filePath.string());
    }

    outFile.close();
}

} // namespace TemplateBuilder
