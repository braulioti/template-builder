#pragma once

#include <functional>
#include <string>
#include <vector>

namespace TemplateBuilder {

class CLIFunctions {
public:
    CLIFunctions() = default;
    ~CLIFunctions() = default;

    static std::string executeFunction(const std::string& functionName, const std::vector<std::string>& arguments);
    static std::string parseFunctionExpression(const std::string& expression, std::function<std::string(const std::string&)> resolveVariable);
    static void processFunctionExpressions(std::string& result, std::function<std::string(const std::string&)> parseFunctionExpr);

private:
};

} // namespace TemplateBuilder
