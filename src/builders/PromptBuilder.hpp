#pragma once

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "types/PromptType.hpp"
#include "types/VariableType.hpp"

namespace TemplateBuilder {

class PromptBuilder {
public:
    PromptBuilder() = default;
    ~PromptBuilder() = default;

    // Main public methods
    std::string getContent(const std::string& content, const std::vector<Variable*>& variables);
    std::string build(Prompt* prompt, const std::vector<Variable*>& variables);

private:
    // Prompt input methods
    void getInputString(PromptInput* promptInput);
    void getChecklist(PromptInput* promptInput);
    void getArrayList(PromptInput* promptInput);

    // Variable resolution
    std::string resolveVariableValue(const std::vector<Variable*>& variables, const std::string& variableName);

    // getContent helpers
    void processPrefixPatterns(std::string& result, const std::vector<Variable*>& variables);
    static void processVariablePlaceholders(std::string& result, const std::vector<Variable*>& variables);
    static void clearUnknownPlaceholders(std::string& result);

    // getChecklist helpers
    static void validateChecklistInput(PromptInput* promptInput);
    static std::string buildChecklistSelectedValues(const std::vector<bool>& selected, const std::vector<std::unique_ptr<PromptInputOption>>& options);

    // getArrayList helpers
    static std::vector<std::string> readArrayListLines();
    static std::string joinLinesWithCrLf(const std::vector<std::string>& lines);

    // build helper
    void processPromptInput(PromptInput* promptInput);
};

} // namespace TemplateBuilder
