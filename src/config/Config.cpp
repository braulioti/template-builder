#include "config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace TemplateBuilder {

namespace {

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
}

} // namespace

std::filesystem::path Config::getExecutableDirectory() {
    std::string path;
#if defined(_WIN32) || defined(_WIN64)
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) return std::filesystem::current_path();
    path = buf;
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len <= 0) return std::filesystem::current_path();
    buf[len] = '\0';
    path = buf;
#elif defined(__APPLE__)
    char buf[PATH_MAX];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) != 0) return std::filesystem::current_path();
    path = buf;
#else
    return std::filesystem::current_path();
#endif
    std::filesystem::path p(path);
    return p.parent_path();
}

Config::Config() {
    m_configPath = getExecutableDirectory() / "config.ini";
    if (!std::filesystem::exists(m_configPath)) {
        std::ofstream f(m_configPath);
        if (f) {
            f << "# Template Builder configuration\n";
            f.close();
        }
    }
    load();
}

std::string Config::get(const std::string& section, const std::string& key, const std::string& defaultValue) {
    const std::string fullKey = section.empty() ? key : section + "." + key;
    auto it = m_values.find(fullKey);
    if (it == m_values.end()) {
        m_values[fullKey] = defaultValue;
        save();
        return defaultValue;
    }
    return it->second;
}

void Config::set(const std::string& section, const std::string& key, const std::string& value) {
    m_values[section + "." + key] = value;
}

void Config::load() {
    m_values.clear();
    std::ifstream f(m_configPath);
    if (!f) return;

    std::string currentSection;
    std::string line;

    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                currentSection = trim(line.substr(1, end - 1));
            }
            continue;
        }

        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));
            if (!key.empty()) {
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                m_values[fullKey] = value;
            }
        }
    }
}

void Config::save() const {
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> bySection;
    for (const auto& [fullKey, value] : m_values) {
        size_t dot = fullKey.find('.');
        std::string section = (dot != std::string::npos) ? fullKey.substr(0, dot) : "";
        std::string key = (dot != std::string::npos) ? fullKey.substr(dot + 1) : fullKey;
        bySection[section].emplace_back(key, value);
    }

    std::ofstream f(m_configPath);
    if (!f) return;

    bool first = true;
    if (bySection.count("")) {
        for (const auto& [k, v] : bySection[""]) f << k << "=" << v << "\n";
        first = false;
    }
    for (const auto& [section, pairs] : bySection) {
        if (section.empty()) continue;
        if (!first) f << "\n";
        first = false;
        f << "[" << section << "]\n";
        for (const auto& [k, v] : pairs) f << k << "=" << v << "\n";
    }
}

} // namespace TemplateBuilder
