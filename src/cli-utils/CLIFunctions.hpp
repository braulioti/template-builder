#pragma once

#include <functional>
#include <string>
#include <vector>

namespace TemplateBuilder {

struct FunctionExpressionParams {
    int parenDepth = 0;
    bool inQuotes = false;
    char quoteChar = '\0';
    bool foundFunc = false;
};

struct ProcessUnquotedContext {
    std::string& result;
    size_t j;
    size_t i;
    size_t startPos;
    size_t& funcStart;
    std::function<std::string(const std::string&)>& parseFunctionExpr;

    ProcessUnquotedContext(std::string& r, size_t j_, size_t i_, size_t startPos_, size_t& funcStart_,
        std::function<std::string(const std::string&)>& parseFn)
        : result(r), j(j_), i(i_), startPos(startPos_), funcStart(funcStart_), parseFunctionExpr(parseFn) {}
};

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
