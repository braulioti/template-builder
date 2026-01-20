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

    // Function parsing and execution
    std::string parseFunctionExpression(const std::vector<Variable*>& variables, const std::string& expression);
    std::string executeFunction(const std::string& functionName, const std::vector<std::string>& arguments);

    // Helper for parsing function arguments
    std::string parseArgument(const std::vector<Variable*>& variables, const std::string& argStr);
};

} // namespace TemplateBuilder
