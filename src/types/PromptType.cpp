#include "types/PromptType.hpp"
#include <algorithm>
#include <stdexcept>

namespace TemplateBuilder {

// PromptInputOption implementation
PromptInputOption::PromptInputOption(const std::string& name, const std::string& value)
    : m_name(name), m_value(value) {
}

// PromptInput implementation
PromptInput::PromptInput() = default;

PromptInput::PromptInput(PromptType type)
    : m_type(type) {
}

void PromptInput::addOption(std::unique_ptr<PromptInputOption> option) {
    if (option) {
        m_options.push_back(std::move(option));
    }
}

void PromptInput::addOption(const std::string& name, const std::string& value) {
    m_options.push_back(std::make_unique<PromptInputOption>(name, value));
}

void PromptInput::clearOptions() {
    m_options.clear();
}

PromptType PromptInput::stringToType(const std::string& typeStr) {
    std::string lowerTypeStr = typeStr;
    std::transform(lowerTypeStr.begin(), lowerTypeStr.end(), lowerTypeStr.begin(), ::tolower);

    if (lowerTypeStr == "inputstring") {
        return PromptType::ptInputString;
    } else if (lowerTypeStr == "checklist") {
        return PromptType::ptChecklist;
    } else if (lowerTypeStr == "arraylist") {
        return PromptType::ptArrayList;
    }

    throw std::invalid_argument("Unknown prompt type: " + typeStr);
}

// Prompt implementation
Prompt::Prompt() = default;

Prompt::Prompt(const std::string& name)
    : m_name(name) {
}

void Prompt::addInput(std::unique_ptr<PromptInput> input) {
    if (input) {
        m_inputs.push_back(std::move(input));
    }
}

void Prompt::clearInputs() {
    m_inputs.clear();
}

} // namespace TemplateBuilder
