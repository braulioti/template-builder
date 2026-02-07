#pragma once

#include <string>

namespace TemplateBuilder {

struct ArgumentParserResult {
    std::string yamlFilePath;
    bool forceInteractive = false;
    bool showHelp = false;
    bool valid = false;

    [[nodiscard]] const std::string& getYamlFilePath() const noexcept { return yamlFilePath; }
};

class ArgumentParserService {
public:
    ArgumentParserService() = default;
    ~ArgumentParserService() = default;

    [[nodiscard]] ArgumentParserResult parse(int argc, char* argv[]);
    static void showUsage(const char* programName);
};

} // namespace TemplateBuilder
