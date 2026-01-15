#pragma once

#include <string>
#include <vector>
#include <memory>
#include "types/VariableType.hpp"

namespace TemplateBuilder {

enum class PromptType {
    ptInputString,
    ptChecklist,
    ptArrayList
};

class PromptInputOption {
public:
    // Constructors
    PromptInputOption() = default;
    PromptInputOption(const std::string& name, const std::string& value);

    // Getters
    [[nodiscard]] const std::string& getName() const noexcept { return m_name; }
    [[nodiscard]] const std::string& getValue() const noexcept { return m_value; }

    // Setters
    void setName(const std::string& name) { m_name = name; }
    void setValue(const std::string& value) { m_value = value; }

private:
    std::string m_name;
    std::string m_value;
};

class PromptInput {
public:
    // Constructors
    PromptInput();
    explicit PromptInput(PromptType type);

    // Getters
    [[nodiscard]] Variable* getVariable() const noexcept { return m_variable; }
    [[nodiscard]] const std::string& getInput() const noexcept { return m_input; }
    [[nodiscard]] PromptType getType() const noexcept { return m_type; }
    [[nodiscard]] const std::vector<std::unique_ptr<PromptInputOption>>& getOptions() const noexcept { return m_options; }
    [[nodiscard]] std::vector<std::unique_ptr<PromptInputOption>>& getOptions() noexcept { return m_options; }

    // Setters
    void setVariable(Variable* variable) { m_variable = variable; }
    void setInput(const std::string& input) { m_input = input; }
    void setType(PromptType type) { m_type = type; }

    // Options management
    void addOption(std::unique_ptr<PromptInputOption> option);
    void addOption(const std::string& name, const std::string& value);
    void clearOptions();
    [[nodiscard]] size_t getOptionsCount() const noexcept { return m_options.size(); }

    // Conversion helper
    [[nodiscard]] static PromptType stringToType(const std::string& typeStr);

private:
    Variable* m_variable = nullptr;  // Non-owning pointer
    std::string m_input;
    PromptType m_type = PromptType::ptInputString;
    std::vector<std::unique_ptr<PromptInputOption>> m_options;  // Owning vector
};

class Prompt {
public:
    // Constructors
    Prompt();
    explicit Prompt(const std::string& name);

    // Getters
    [[nodiscard]] const std::string& getName() const noexcept { return m_name; }
    [[nodiscard]] const std::string& getResult() const noexcept { return m_result; }
    [[nodiscard]] const std::vector<std::unique_ptr<PromptInput>>& getInputs() const noexcept { return m_inputs; }
    [[nodiscard]] std::vector<std::unique_ptr<PromptInput>>& getInputs() noexcept { return m_inputs; }

    // Setters
    void setName(const std::string& name) { m_name = name; }
    void setResult(const std::string& result) { m_result = result; }

    // Inputs management
    void addInput(std::unique_ptr<PromptInput> input);
    void clearInputs();
    [[nodiscard]] size_t getInputsCount() const noexcept { return m_inputs.size(); }

private:
    std::string m_name;
    std::string m_result;
    std::vector<std::unique_ptr<PromptInput>> m_inputs;  // Owning vector
};

} // namespace TemplateBuilder
