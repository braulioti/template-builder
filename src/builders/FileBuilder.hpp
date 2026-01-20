#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include "types/FileType.hpp"
#include "builders/PromptBuilder.hpp"

namespace TemplateBuilder {

class FileBuilder {
public:
    FileBuilder();
    ~FileBuilder() = default;

    // Main method to build a file
    void build(const FileData* file);

private:
    // Helper methods to reduce complexity
    void validateFile(const FileData* file) const;
    std::filesystem::path prepareFilePath(const FileData* file) const;
    void ensureDirectoryExists(const std::filesystem::path& filePath) const;
    std::string getFileContent(const FileData* file) const;
    std::vector<Variable*> getVariableList(const FileData* file) const;
    void writeFile(const std::filesystem::path& filePath, const std::string& content) const;

    std::unique_ptr<PromptBuilder> m_promptBuilder;
};

} // namespace TemplateBuilder
