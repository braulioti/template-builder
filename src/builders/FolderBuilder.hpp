#pragma once

#include <filesystem>
#include "types/FileType.hpp"

namespace TemplateBuilder {

class FolderBuilder {
public:
    FolderBuilder() = default;
    ~FolderBuilder() = default;

    // Main method to build a folder/directory
    void build(const FileData* folder);

private:
    // Helper methods to reduce complexity
    void validateFolder(const FileData* folder) const;
    std::filesystem::path prepareFolderPath(const FileData* folder) const;
    std::filesystem::path determineDirectoryPath(const std::filesystem::path& fullPath, const std::string& originalPath) const;
    void createDirectory(const std::filesystem::path& directory) const;
};

} // namespace TemplateBuilder
