#include "builders/RemoteFileBuilder.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <curl/curl.h>

namespace TemplateBuilder {

// Callback function to write data to file
static size_t WriteCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* out = static_cast<std::ofstream*>(userdata);
    out->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

RemoteFileBuilder::RemoteFileBuilder() {
    // Initialize libcurl (idempotent, safe to call multiple times)
    static bool curlInitialized = false;
    if (!curlInitialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlInitialized = true;
        
        // Check if HTTPS support is available
        CURL* testCurl = curl_easy_init();
        if (testCurl) {
            curl_version_info_data* versionInfo = curl_version_info(CURLVERSION_NOW);
            if (versionInfo && !(versionInfo->features & CURL_VERSION_SSL)) {
                std::cerr << "WARNING: libcurl was compiled without SSL/TLS support. HTTPS downloads will fail." << std::endl;
                std::cerr << "Please rebuild libcurl with SSL/TLS support." << std::endl;
            }
            curl_easy_cleanup(testCurl);
        }
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

namespace {

void setCurlOptions(CURL* curl, const std::string& uri, std::ofstream* outFile) {
    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, outFile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 TemplateBuilder/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
}

void removeFileIfExists(const std::filesystem::path& filePath) {
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}

bool checkDownloadResult(CURLcode res, long httpCode, const std::filesystem::path& filePath) {
    if (res != CURLE_OK) {
        removeFileIfExists(filePath);
        std::cerr << " (CURL error: " << curl_easy_strerror(res) << ")" << std::endl;
        return false;
    }
    if (httpCode != 200) {
        removeFileIfExists(filePath);
        std::cerr << " (HTTP " << httpCode << ")" << std::endl;
        return false;
    }
    bool fileWritten = std::filesystem::exists(filePath) && std::filesystem::file_size(filePath) > 0;
    if (!fileWritten) {
        removeFileIfExists(filePath);
        std::cerr << " (File was not written or is empty)" << std::endl;
        return false;
    }
    return true;
}

} // namespace

bool RemoteFileBuilder::downloadFile(const std::string& uri, const std::filesystem::path& filePath) const {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) {
        curl_easy_cleanup(curl);
        std::cerr << " (Failed to open file for writing: " << filePath.string() << ")" << std::endl;
        return false;
    }

    setCurlOptions(curl, uri, &outFile);
    CURLcode res = curl_easy_perform(curl);
    outFile.flush();
    outFile.close();

    long httpCode = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }
    curl_easy_cleanup(curl);

    return checkDownloadResult(res, httpCode, filePath);
}

void RemoteFileBuilder::showDownloadStatus(const std::string& path) const {
    std::cout << "Downloading... " << path;
    std::cout.flush();
}

void RemoteFileBuilder::showSuccessStatus() const {
    std::cout << " <OK>" << std::endl;
}

} // namespace TemplateBuilder
