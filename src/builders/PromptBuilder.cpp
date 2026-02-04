#include "builders/PromptBuilder.hpp"
#include "cli-utils/CLIFunctions.hpp"
#include "cli-utils/CLINavigate.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <limits>
#include <cstdio>

#ifdef _WIN32
#include <io.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace TemplateBuilder {

static bool stdinIsInteractive() {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(fileno(stdin)) != 0;
#endif
}

void PromptBuilder::getInputString(PromptInput* promptInput) {
    if (!promptInput) {
        return;
    }

    if (!promptInput->getVariable()) {
        throw std::runtime_error("Variable is nullptr in PromptInput.");
    }

    try {
        // In non-interactive environments (unit tests/CI), don't block waiting for input.
        // Keep an existing value if present; otherwise default to empty.
        if (!stdinIsInteractive()) {
            if (!promptInput->getVariable()->hasValue()) {
                promptInput->getVariable()->setValue("");
            }
            return;
        }

        // Output the prompt text and flush to ensure it's displayed immediately (including trailing spaces)
        // Using std::flush ensures the entire prompt text (including trailing spaces) is displayed before reading input
        // Note: std::cout is line-buffered by default, so we need flush to ensure immediate display
        const std::string& inputText = promptInput->getInput();
        std::cout.write(inputText.c_str(), inputText.size());
        std::cout.flush();
        
        // Check if std::cin is in a good state before reading
        if (!std::cin.good()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        
        std::string userInput;
        if (!std::getline(std::cin, userInput)) {
            // If getline fails, use empty string
            userInput = "";
        }
        
        // Store the value - std::getline already consumes the newline, so no extra newline is added
        promptInput->getVariable()->setValue(userInput);
    } catch (const std::exception& e) {
        // Re-throw with more context
        throw std::runtime_error("Error reading input: " + std::string(e.what()));
    }
}

void PromptBuilder::validateChecklistInput(PromptInput* promptInput) {
    if (!promptInput || !promptInput->getVariable()) {
        throw std::runtime_error("Variable is nullptr in PromptInput.");
    }
    if (promptInput->getOptions().empty()) {
        throw std::runtime_error("No options available for checklist input.");
    }
}

std::string PromptBuilder::buildChecklistSelectedValues(const std::vector<bool>& selected, const std::vector<std::unique_ptr<PromptInputOption>>& options) {
    std::string selectedValues;
    for (size_t i = 0; i < options.size(); ++i) {
        if (selected[i]) {
            if (!selectedValues.empty()) {
                selectedValues += "\r\n";
            }
            selectedValues += options[i]->getValue();
        }
    }
    return selectedValues;
}

void PromptBuilder::getChecklist(PromptInput* promptInput) {
    if (!promptInput) {
        return;
    }
    validateChecklistInput(promptInput);
    if (!stdinIsInteractive()) {
        if (!promptInput->getVariable()->hasValue()) {
            promptInput->getVariable()->setValue("");
        }
        return;
    }
    const auto& options = promptInput->getOptions();
    std::vector<bool> selected(options.size(), false);
    size_t currentIndex = 0;
    bool done = false;
    std::cout << std::endl << promptInput->getInput() << std::endl << std::endl;
    ChecklistLoopParams params(promptInput, selected, currentIndex, done);
    CLINavigate::runChecklistLoop(params);
    std::cout << "\033[A\033[2K" << std::endl;
    promptInput->getVariable()->setValue(buildChecklistSelectedValues(selected, options));
}

std::vector<std::string> PromptBuilder::readArrayListLines() {
    std::vector<std::string> lines;
    while (true) {
        std::cout << "> ";
        std::string userInput;
        std::getline(std::cin, userInput);
        userInput.erase(0, userInput.find_first_not_of(" \t\n\r"));
        if (!userInput.empty()) {
            userInput.erase(userInput.find_last_not_of(" \t\n\r") + 1);
        }
        if (userInput.empty()) {
            break;
        }
        lines.push_back(userInput);
    }
    return lines;
}

std::string PromptBuilder::joinLinesWithCrLf(const std::vector<std::string>& lines) {
    std::string result;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) {
            result += "\r\n";
        }
        result += lines[i];
    }
    return result;
}

void PromptBuilder::getArrayList(PromptInput* promptInput) {
    if (!promptInput) {
        return;
    }
    if (!promptInput->getVariable()) {
        throw std::runtime_error("Variable is nullptr in PromptInput.");
    }
    if (!stdinIsInteractive()) {
        if (!promptInput->getVariable()->hasValue()) {
            promptInput->getVariable()->setValue("");
        }
        return;
    }
    std::cout << std::endl << promptInput->getInput() << std::endl
              << "Enter each option and press Enter. Leave empty and press Enter to finish:" << std::endl << std::endl;
    std::vector<std::string> lines = readArrayListLines();
    promptInput->getVariable()->setValue(joinLinesWithCrLf(lines));
    std::cout << std::endl;
}

std::string PromptBuilder::resolveVariableValue(const std::vector<Variable*>& variables, const std::string& variableName) {
    if (variables.empty()) {
        return "";
    }

    for (Variable* variable : variables) {
        if (variable && variable->getName() == variableName) {
            if (variable->hasValue()) {
                return variable->getValue();
            }
        }
    }

    return "";
}

static std::string getTrimmedVariableValue(Variable* variable) {
    if (!variable || !variable->hasValue()) {
        return "";
    }
    std::string v = variable->getValue();
    if (v.empty()) {
        return v;
    }
    v.erase(0, v.find_first_not_of(" \t\n\r"));
    if (!v.empty()) {
        v.erase(v.find_last_not_of(" \t\n\r") + 1);
    }
    return v;
}

static std::vector<std::string> splitVariableValueIntoLines(const std::string& variableValue) {
    std::vector<std::string> lines;
    std::istringstream iss(variableValue);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}

static std::string joinLinesWithPrefix(const std::vector<std::string>& lines, const std::string& prefix) {
    std::string processedValue;
    for (size_t j = 0; j < lines.size(); ++j) {
        if (j > 0) {
            processedValue += "\r\n";
        }
        processedValue += prefix + lines[j];
    }
    return processedValue;
}

void PromptBuilder::processPrefixPatterns(std::string& result, const std::vector<Variable*>& variables) {
    std::regex prefixPattern("\\{\\{\"([^\"]+)\"\\s*\\|\\s*(\\w+)\\}\\}");
    std::sregex_iterator iter(result.begin(), result.end(), prefixPattern);
    std::sregex_iterator end;
    std::vector<std::pair<size_t, size_t>> matches;
    std::vector<std::pair<std::string, std::string>> replacements;
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        matches.push_back({match.position(), match.length()});
        replacements.push_back({match[1].str(), match[2].str()});
    }
    for (int i = static_cast<int>(matches.size()) - 1; i >= 0; --i) {
        size_t pos = matches[i].first;
        size_t len = matches[i].second;
        std::string prefix = replacements[i].first;
        std::string varName = replacements[i].second;
        std::string variableValue = resolveVariableValue(variables, varName);
        std::vector<std::string> lines = splitVariableValueIntoLines(variableValue);
        std::string processedValue = joinLinesWithPrefix(lines, prefix);
        result.replace(pos, len, processedValue);
    }
}

void PromptBuilder::processVariablePlaceholders(std::string& result, const std::vector<Variable*>& variables) {
    for (Variable* variable : variables) {
        if (!variable) {
            continue;
        }
        std::string variableName = variable->getName();
        std::string variableValue = getTrimmedVariableValue(variable);
        std::string placeholder = "{{" + variableName + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), variableValue);
            pos += variableValue.length();
        }
    }
}

void PromptBuilder::clearUnknownPlaceholders(std::string& result) {
    static const std::regex unknownPlaceholderPattern(R"(\{\{([A-Za-z_][A-Za-z0-9_]*)\}\})");
    result = std::regex_replace(result, unknownPlaceholderPattern, "");
}

std::string PromptBuilder::getContent(const std::string& content, const std::vector<Variable*>& variables) {
    try {
        if (content.empty() || variables.empty()) {
            return content;
        }
        std::string result = content;
        processPrefixPatterns(result, variables);
        CLIFunctions::processFunctionExpressions(result, [this, &variables](const std::string& expr) {
            return CLIFunctions::parseFunctionExpression(expr, [this, &variables](const std::string& name) {
                return resolveVariableValue(variables, name);
            });
        });
        processVariablePlaceholders(result, variables);
        clearUnknownPlaceholders(result);
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error processing content template: " + std::string(e.what()));
    }
}

void PromptBuilder::processPromptInput(PromptInput* promptInput) {
    PromptType type = promptInput->getType();
    if (type == PromptType::ptInputString) {
        getInputString(promptInput);
    } else if (type == PromptType::ptChecklist) {
        getChecklist(promptInput);
    } else if (type == PromptType::ptArrayList) {
        getArrayList(promptInput);
    }
}

std::string PromptBuilder::build(Prompt* prompt, const std::vector<Variable*>& variables) {
    if (!prompt) {
        return "";
    }
    try {
        for (const auto& promptInput : prompt->getInputs()) {
            if (promptInput) {
                try {
                    processPromptInput(promptInput.get());
                } catch (const std::exception& e) {
                    throw std::runtime_error("Error processing prompt input: " + std::string(e.what()));
                }
            }
        }
        return getContent(prompt->getResult(), variables);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error building prompt: " + std::string(e.what()));
    }
}

} // namespace TemplateBuilder
