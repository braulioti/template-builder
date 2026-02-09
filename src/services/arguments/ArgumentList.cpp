#include "services/arguments/ArgumentList.hpp"
#include "cli-utils/CLINavigate.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <optional>
#include <curl/curl.h>

namespace TemplateBuilder {

namespace {

size_t writeToString(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* str = static_cast<std::string*>(userdata);
    const size_t total = size * nmemb;
    str->append(static_cast<char*>(ptr), total);
    return total;
}

void ensureCurlInit() {
    static bool initialized = false;
    if (initialized) return;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    initialized = true;
}

std::optional<std::string> performDownload(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: Failed to initialize download." << std::endl;
        return std::nullopt;
    }
    std::string body;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "TemplateBuilder/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    if (res == CURLE_OK) curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error downloading list: " << curl_easy_strerror(res) << std::endl;
        return std::nullopt;
    }
    if (httpCode != 200) {
        std::cerr << "Error: List URL returned HTTP " << httpCode << std::endl;
        return std::nullopt;
    }
    return body;
}

std::vector<std::string> parseLinesFromBody(const std::string& body) {
    std::vector<std::string> lines;
    std::istringstream stream(body);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

void printSelectedValue(const std::vector<std::string>& lines, size_t index) {
    const std::string& line = lines[index];
    const size_t pos = line.find(';');
    const std::string value = (pos != std::string::npos) ? line.substr(0, pos) : line;
    std::cout << value << std::endl;
}

} // namespace

ArgumentList::ArgumentList(int argc, char* argv[]) {
    m_args.reserve(static_cast<size_t>(argc > 0 ? argc - 1 : 0));
    for (int i = 1; i < argc; ++i) {
        if (argv[i]) {
            m_args.emplace_back(argv[i]);
        }
    }
}

const std::string& ArgumentList::at(size_t index) const {
    if (index >= m_args.size()) {
        throw std::out_of_range("ArgumentList::at index out of range");
    }
    return m_args[index];
}

bool ArgumentList::fetchAndPrintList(const std::string& listUrl) {
    if (listUrl.empty()) {
        std::cerr << "Error: List URL is empty." << std::endl;
        return false;
    }
    ensureCurlInit();
    auto bodyOpt = performDownload(listUrl);
    if (!bodyOpt) return false;

    std::vector<std::string> lines = parseLinesFromBody(*bodyOpt);
    if (lines.empty()) {
        std::cerr << "Error: List is empty." << std::endl;
        return false;
    }

    auto selected = CLINavigate::selectFromList(lines);
    if (selected.has_value()) printSelectedValue(lines, *selected);
    return true;
}

} // namespace TemplateBuilder
