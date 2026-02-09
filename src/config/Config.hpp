#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

namespace TemplateBuilder {

/**
 * Reads and writes config.ini in the same directory as the executable.
 * If the file does not exist at construction time, it is created (empty).
 */
class Config {
public:
    /** Locates config.ini next to the executable and creates it if missing. */
    Config();

    /** Path to the config.ini file (directory of executable + "config.ini"). */
    [[nodiscard]] const std::filesystem::path& getConfigPath() const noexcept { return m_configPath; }

    /**
     * Get value by section and key. If the section or key does not exist,
     * they are created in the file with the default value, and the default is returned.
     * Otherwise returns the value stored in the file.
     */
    [[nodiscard]] std::string get(const std::string& section, const std::string& key, const std::string& defaultValue);

    /** Set a value (in memory). Call save() to write to file. */
    void set(const std::string& section, const std::string& key, const std::string& value);

    /** Reload from file. */
    void load();

    /** Write current in-memory values to config.ini. */
    void save() const;

private:
    std::filesystem::path m_configPath;
    std::unordered_map<std::string, std::string> m_values;  // "section.key" or "key" -> value

    static std::filesystem::path getExecutableDirectory();
};

} // namespace TemplateBuilder
