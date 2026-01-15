#pragma once

#include <string>
#include <vector>
#include <memory>
#include "types/PromptType.hpp"
#include "types/VariableType.hpp"

namespace TemplateBuilder {

class FileData {
public:
    // Constructors
    FileData() = default;
    FileData(const std::string& path, const std::string& content);

    // Getters
    [[nodiscard]] const std::string& getPath() const noexcept { return m_path; }
    [[nodiscard]] const std::string& getContent() const noexcept { return m_content; }
    [[nodiscard]] Prompt* getPrompt() const noexcept { return m_prompt; }
    [[nodiscard]] const std::vector<Variable*>* getVariables() const noexcept { return m_variables; }

    // Setters
    void setPath(const std::string& path) { m_path = path; }
    void setContent(const std::string& content) { m_content = content; }
    void setPrompt(Prompt* prompt) { m_prompt = prompt; }
    void setVariables(const std::vector<Variable*>* variables) { m_variables = variables; }

    // Utility methods
    [[nodiscard]] bool hasPrompt() const noexcept { return m_prompt != nullptr; }
    [[nodiscard]] bool hasVariables() const noexcept { return m_variables != nullptr; }
    [[nodiscard]] bool isEmpty() const noexcept { return m_path.empty() && m_content.empty(); }

private:
    std::string m_path;
    std::string m_content;
    Prompt* m_prompt = nullptr;  // Non-owning pointer
    const std::vector<Variable*>* m_variables = nullptr;  // Non-owning pointer to shared vector
};

} // namespace TemplateBuilder
