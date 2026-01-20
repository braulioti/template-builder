#pragma once

#include <string>
#include <filesystem>
#include "types/RemoteFileType.hpp"

namespace TemplateBuilder {

class RemoteFileBuilder {
public:
    RemoteFileBuilder();
    ~RemoteFileBuilder();

    // Main method to download and save a remote file
    void build(const RemoteFileData* remoteFile);

private:
    // Helper methods to reduce complexity
    void validateRemoteFile(const RemoteFileData* remoteFile) const;
    std::filesystem::path prepareFilePath(const RemoteFileData* remoteFile) const;
    void ensureDirectoryExists(const std::filesystem::path& filePath) const;
    bool downloadFile(const std::string& uri, const std::filesystem::path& filePath) const;
    void showDownloadStatus(const std::string& path) const;
    void showSuccessStatus() const;
};

} // namespace TemplateBuilder
