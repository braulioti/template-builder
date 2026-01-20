#include "builders/RemoteFileBuilder.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <curl/curl.h>

namespace TemplateBuilder {

// Callback function to write data to file
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* stream = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;
    stream->write(static_cast<const char*>(contents), totalSize);
    return totalSize;
}

RemoteFileBuilder::RemoteFileBuilder() {
    // Initialize libcurl (idempotent, safe to call multiple times)
    static bool curlInitialized = false;
    if (!curlInitialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlInitialized = true;
    }
}

RemoteFileBuilder::~RemoteFileBuilder() {
    // Note: curl_global_cleanup should be called at program exit
    // We don't clean up here to allow multiple instances
}

void RemoteFileBuilder::build(const RemoteFileData* remoteFile) {
    if (!remoteFile) {
        return;
    }

    validateRemoteFile(remoteFile);
    std::filesystem::path filePath = prepareFilePath(remoteFile);
    ensureDirectoryExists(filePath);
    
    showDownloadStatus(remoteFile->getPath());
    
    if (!downloadFile(remoteFile->getUri(), filePath)) {
        throw std::runtime_error("Failed to download remote file: " + remoteFile->getUri());
    }
    
    showSuccessStatus();
}

void RemoteFileBuilder::validateRemoteFile(const RemoteFileData* remoteFile) const {
    if (!remoteFile) {
        throw std::invalid_argument("Remote file cannot be null.");
    }
    
    if (remoteFile->getUri().empty()) {
        throw std::invalid_argument("Remote file URI cannot be empty.");
    }
    
    if (remoteFile->getPath().empty()) {
        throw std::invalid_argument("Remote file path cannot be empty.");
    }
}

std::filesystem::path RemoteFileBuilder::prepareFilePath(const RemoteFileData* remoteFile) const {
    std::filesystem::path currentDir = std::filesystem::current_path();
    return currentDir / remoteFile->getPath();
}

void RemoteFileBuilder::ensureDirectoryExists(const std::filesystem::path& filePath) const {
    std::filesystem::path directory = filePath.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
}

bool RemoteFileBuilder::downloadFile(const std::string& uri, const std::filesystem::path& filePath) const {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    
    long httpCode = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }

    outFile.close();
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || httpCode != 200) {
        // Clean up partial file on error
        if (std::filesystem::exists(filePath)) {
            std::filesystem::remove(filePath);
        }
        return false;
    }

    return true;
}

void RemoteFileBuilder::showDownloadStatus(const std::string& path) const {
    std::cout << "Downloading... " << path;
    std::cout.flush();
}

void RemoteFileBuilder::showSuccessStatus() const {
    std::cout << " <OK>" << std::endl;
}

} // namespace TemplateBuilder
