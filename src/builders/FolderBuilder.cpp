#include "builders/FolderBuilder.hpp"
#include <stdexcept>

namespace TemplateBuilder {

void FolderBuilder::build(const FileData* folder) {
    validateFolder(folder);
    std::filesystem::path fullPath = prepareFolderPath(folder);
    std::filesystem::path directory = determineDirectoryPath(fullPath, folder->getPath());
    createDirectory(directory);
}

void FolderBuilder::validateFolder(const FileData* folder) const {
    if (!folder) {
        return;
    }

    if (folder->getPath().empty()) {
        throw std::runtime_error("Folder path cannot be empty.");
    }
}

std::filesystem::path FolderBuilder::prepareFolderPath(const FileData* folder) const {
    std::filesystem::path currentDir = std::filesystem::current_path();
    return currentDir / folder->getPath();
}

std::filesystem::path FolderBuilder::determineDirectoryPath(const std::filesystem::path& fullPath, const std::string& originalPath) const {
    // Check if path ends with a directory separator (it's explicitly a directory path)
    if (!originalPath.empty()) {
        char lastChar = originalPath.back();
        if (lastChar == '/' || lastChar == '\\') {
            // It's explicitly a directory path, use fullPath directly
            return fullPath;
        }
    }
    
    // Get the directory part of the path (same logic as FileBuilder)
    return fullPath.parent_path();
}

void FolderBuilder::createDirectory(const std::filesystem::path& directory) const {
    // Create the directory structure (only creates if it doesn't exist)
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
}

} // namespace TemplateBuilder
