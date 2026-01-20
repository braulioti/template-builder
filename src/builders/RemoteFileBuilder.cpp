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

    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
    
    // Set write callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
    
    // Follow redirects (important for GitHub raw URLs)
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    
    // Set User-Agent (some servers like GitHub require this)
    // Using a more realistic User-Agent to avoid blocking
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 TemplateBuilder/1.0");
    
    // SSL options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    
    // Timeout options
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    
    // Flush and close file before checking results
    outFile.flush();
    outFile.close();
    
    long httpCode = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }
    
    // Check if file was written successfully
    bool fileExists = std::filesystem::exists(filePath);
    bool fileWritten = fileExists && std::filesystem::file_size(filePath) > 0;

    if (res != CURLE_OK) {
        // Get error message from curl
        const char* errorMsg = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        
        // Clean up partial file on error
        if (fileExists) {
            std::filesystem::remove(filePath);
        }
        
        std::cerr << " (CURL error: " << errorMsg << ")" << std::endl;
        return false;
    }

    curl_easy_cleanup(curl);

    if (httpCode != 200) {
        // Clean up partial file on error
        if (fileExists) {
            std::filesystem::remove(filePath);
        }
        std::cerr << " (HTTP " << httpCode << ")" << std::endl;
        return false;
    }

    if (!fileWritten) {
        // Clean up empty file
        if (fileExists) {
            std::filesystem::remove(filePath);
        }
        std::cerr << " (File was not written or is empty)" << std::endl;
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
