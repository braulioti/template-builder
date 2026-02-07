#pragma once

#include <string>

namespace TemplateBuilder {

struct ArgumentParserResult {
    std::string yamlFilePath;
    bool forceInteractive = false;
    bool showHelp = false;
    bool valid = false;
};

class ArgumentParserService {
public:
    ArgumentParserService() = default;
    ~ArgumentParserService() = default;

    [[nodiscard]] ArgumentParserResult parse(int argc, char* argv[]);
    static void showUsage(const char* programName);
};

} // namespace TemplateBuilder
