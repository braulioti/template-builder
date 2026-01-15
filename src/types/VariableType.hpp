#pragma once

#include <string>
#include <variant>
#include <optional>

namespace TemplateBuilder {

enum class VariableType {
    vtString
};

class Variable {
public:
    // Constructors
    Variable() = default;
    Variable(const std::string& name, VariableType type);
    Variable(const std::string& name, VariableType type, const std::string& value);

    // Getters
    [[nodiscard]] const std::string& getName() const noexcept { return m_name; }
    [[nodiscard]] VariableType getType() const noexcept { return m_type; }
    [[nodiscard]] const std::string& getValue() const;
    [[nodiscard]] bool hasValue() const noexcept { return m_value.has_value(); }

    // Setters
    void setName(const std::string& name) { m_name = name; }
    void setType(VariableType type) { m_type = type; }
    void setValue(const std::string& value) { m_value = value; }
    void clearValue() { m_value.reset(); }

    // Conversion helper
    [[nodiscard]] static VariableType stringToType(const std::string& typeStr);

private:
    std::string m_name;
    VariableType m_type = VariableType::vtString;
    std::optional<std::string> m_value;
};

} // namespace TemplateBuilder
