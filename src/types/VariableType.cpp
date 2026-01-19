#include "types/VariableType.hpp"
#include <algorithm>
#include <stdexcept>

namespace TemplateBuilder {

Variable::Variable(const std::string& name, VariableType type)
    : m_name(name), m_type(type) {
}

Variable::Variable(const std::string& name, VariableType type, const std::string& value)
    : m_name(name), m_type(type), m_value(value) {
}

const std::string& Variable::getValue() const {
    if (!m_value.has_value()) {
        throw std::runtime_error("Variable value not set");
    }
    return m_value.value();
}

VariableType Variable::stringToType(const std::string& typeStr) {
    std::string lowerTypeStr = typeStr;
    std::transform(lowerTypeStr.begin(), lowerTypeStr.end(), lowerTypeStr.begin(), ::tolower);

    if (lowerTypeStr == "string") {
        return VariableType::vtString;
    }

    throw std::invalid_argument("Unknown variable type: " + typeStr);
}

} // namespace TemplateBuilder
