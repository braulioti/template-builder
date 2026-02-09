#pragma once

#include <string>

namespace TemplateBuilder {

struct ArgumentParserResult {
    std::string yamlFilePath;
    bool forceInteractive = false;
    bool showHelp = false;
    bool showList = false;
    bool valid = false;

    [[nodiscard]] const std::string& getYamlFilePath() const noexcept { return yamlFilePath; }
};

class ArgumentParserService {
public:
    explicit ArgumentParserService(std::string listUrl);
    ~ArgumentParserService() = default;

    [[nodiscard]] ArgumentParserResult parse(int argc, char* argv[]);
    void showUsage(const char* programName) const;

    /** Downloads the list from ListURL, shows interactive selection (Up/Down, Enter), prints selected item. Returns false on error. */
    bool fetchAndPrintList() const;

private:
    std::string m_listUrl;
};

} // namespace TemplateBuilder
