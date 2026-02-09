#include "cli-utils/CLIFunctions.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace TemplateBuilder {

namespace {

std::string trimArg(const std::string& s) {
    std::string t = s;
    t.erase(0, t.find_first_not_of(" \t\n\r"));
    if (!t.empty()) {
        t.erase(t.find_last_not_of(" \t\n\r") + 1);
    }
    return t;
}

std::string toLowerCaseString(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

void expectArgCount(const std::string& functionName, size_t expected, size_t actual) {
    if (actual != expected) {
        throw std::runtime_error("Function '" + functionName + "' expects " +
                                 std::to_string(expected) + " argument(s), got " + std::to_string(actual));
    }
}

std::string executeCaseTransform(const std::string& functionName,
                                 const std::vector<std::string>& arguments,
                                 int (*charTransform)(int)) {
    expectArgCount(functionName, 1, arguments.size());
    std::string result = arguments[0];
    std::transform(result.begin(), result.end(), result.begin(),
                  [charTransform](unsigned char c) { return static_cast<char>(charTransform(c)); });
    return result;
}

std::string executeUpperImpl(const std::vector<std::string>& arguments) {
    return executeCaseTransform("upper", arguments, std::toupper);
}

std::string executeLowerImpl(const std::vector<std::string>& arguments) {
    return executeCaseTransform("lower", arguments, std::tolower);
}

std::string executeReplaceImpl(const std::vector<std::string>& arguments) {
    expectArgCount("replace", 3, arguments.size());
    std::string result = arguments[2];
    const std::string& search = arguments[0];
    const std::string& replacement = arguments[1];
    size_t pos = 0;
    while ((pos = result.find(search, pos)) != std::string::npos) {
        result.replace(pos, search.length(), replacement);
        pos += replacement.length();
    }
    return result;
}

bool isQuotedString(const std::string& s) {
    return s.length() >= 2 &&
           ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''));
}

std::string unescapeQuotedString(const std::string& s) {
    std::string result = s.substr(1, s.length() - 2);
    const std::string escaped = (s.front() == '"') ? "\"\"" : "''";
    const char replacement = s.front();
    size_t pos = 0;
    while ((pos = result.find(escaped, pos)) != std::string::npos) {
        result.replace(pos, 2, 1, replacement);
        pos += 1;
    }
    return result;
}

struct ArgumentParserState {
    std::vector<std::string> arguments;
    size_t startPos = 0;
    bool inQuotes = false;
    char quoteChar = '\0';
    int functionDepth = 0;
};

void addArgumentIfNonEmpty(ArgumentParserState& state, const std::string& argsString, size_t endPos) {
    std::string arg = trimArg(argsString.substr(state.startPos, endPos - state.startPos));
    if (!arg.empty()) {
        state.arguments.push_back(arg);
    }
}

void processUnquotedChar(ArgumentParserState& state, const std::string& argsString, size_t j) {
    char c = argsString[j];
    if (c == '"' || c == '\'') {
        state.inQuotes = true;
        state.quoteChar = c;
        return;
    }
    if (c == '(') {
        state.functionDepth++;
        return;
    }
    if (c == ')') {
        state.functionDepth--;
        return;
    }
    if (c == ',' && state.functionDepth == 0) {
        addArgumentIfNonEmpty(state, argsString, j);
        state.startPos = j + 1;
    }
}

bool processQuotedChar(ArgumentParserState& state, const std::string& argsString, size_t j) {
    if (argsString[j] != state.quoteChar) {
        return false;
    }
    if (j + 1 < argsString.length() && argsString[j + 1] == state.quoteChar) {
        return true;  // escaped quote, skip next char
    }
    state.inQuotes = false;
    return false;
}

std::vector<std::string> parseArgumentsFromString(const std::string& argsString) {
    ArgumentParserState state;
    if (argsString.empty()) {
        return state.arguments;
    }
    for (size_t j = 0; j < argsString.length(); ++j) {
        if (state.inQuotes) {
            if (processQuotedChar(state, argsString, j)) {
                ++j;
            }
        } else {
            processUnquotedChar(state, argsString, j);
        }
    }
    addArgumentIfNonEmpty(state, argsString, argsString.length());
    return state.arguments;
}

std::string parseArgument(const std::string& argStr, std::function<std::string(const std::string&)> resolveVariable,
                         std::function<std::string(const std::string&)> parseFunctionExpr) {
    std::string trimmed = trimArg(argStr);
    if (isQuotedString(trimmed)) {
        return unescapeQuotedString(trimmed);
    }
    if (trimmed.find('(') != std::string::npos) {
        return parseFunctionExpr(trimmed);
    }
    return resolveVariable(trimmed);
}

} // namespace

std::string CLIFunctions::parseFunctionExpression(const std::string& expression, std::function<std::string(const std::string&)> resolveVariable) {
    size_t openParenPos = expression.find('(');
    if (openParenPos == std::string::npos) {
        throw std::runtime_error("Invalid function expression: " + expression);
    }
    std::string functionName = trimArg(expression.substr(0, openParenPos));
    std::string argsString = expression.substr(openParenPos + 1);
    while (!argsString.empty() && argsString.back() == ')') {
        argsString.pop_back();
    }
    argsString = trimArg(argsString);
    std::vector<std::string> parsedArguments = parseArgumentsFromString(argsString);
    std::vector<std::string> resolvedArguments;
    for (const auto& arg : parsedArguments) {
        resolvedArguments.push_back(parseArgument(arg, resolveVariable, [resolveVariable](const std::string& expr) {
            return CLIFunctions::parseFunctionExpression(expr, resolveVariable);
        }));
    }
    return executeFunction(functionName, resolvedArguments);
}

std::string CLIFunctions::executeFunction(const std::string& functionName, const std::vector<std::string>& arguments) {
    const std::string lowerName = toLowerCaseString(functionName);
    if (lowerName == "upper") return executeUpperImpl(arguments);
    if (lowerName == "lower") return executeLowerImpl(arguments);
    if (lowerName == "replace") return executeReplaceImpl(arguments);
    throw std::runtime_error("Unknown function: " + functionName);
}

enum class UnquotedAction { Continue, Break, ProcessedAndBreak };

bool isClosingBraces(const ProcessUnquotedContext& ctx) {
    return ctx.j + 1 < ctx.result.length() && ctx.result[ctx.j] == '}' && ctx.result[ctx.j + 1] == '}';
}

bool isEndOfFunctionCall(const ProcessUnquotedContext& ctx) {
    return ctx.j + 2 < ctx.result.length() && ctx.result[ctx.j + 1] == '}' && ctx.result[ctx.j + 2] == '}';
}

UnquotedAction handleQuoteInUnquoted(FunctionExpressionParams& params, ProcessUnquotedContext& ctx) {
    params.inQuotes = true;
    params.quoteChar = ctx.result[ctx.j];
    return UnquotedAction::Continue;
}

UnquotedAction handleOpenParenInUnquoted(FunctionExpressionParams& params, ProcessUnquotedContext& ctx) {
    if (params.parenDepth == 0) {
        ctx.funcStart = ctx.i + 2;
        params.foundFunc = true;
    }
    ++params.parenDepth;
    return UnquotedAction::Continue;
}

UnquotedAction handleCloseParenInUnquoted(FunctionExpressionParams& params, ProcessUnquotedContext& ctx) {
    --params.parenDepth;
    if (params.parenDepth == 0 && params.foundFunc && isEndOfFunctionCall(ctx)) {
        std::string functionExpression = ctx.result.substr(ctx.funcStart, ctx.j - ctx.funcStart + 1);
        std::string functionResult = ctx.parseFunctionExpr(functionExpression);
        ctx.result.replace(ctx.startPos, ctx.j + 3 - ctx.startPos, functionResult);
        return UnquotedAction::ProcessedAndBreak;
    }
    return UnquotedAction::Continue;
}

UnquotedAction handleUnquotedCharInTemplate(FunctionExpressionParams& params, ProcessUnquotedContext& ctx) {
    const char c = ctx.result[ctx.j];
    if (c == '"' || c == '\'') return handleQuoteInUnquoted(params, ctx);
    if (c == '(') return handleOpenParenInUnquoted(params, ctx);
    if (c == ')') return handleCloseParenInUnquoted(params, ctx);
    if (isClosingBraces(ctx) && !params.foundFunc) return UnquotedAction::Break;
    return UnquotedAction::Continue;
}

static size_t findNextExpressionStart(const std::string& result, size_t startIndex) {
    for (size_t i = startIndex; i + 2 <= result.length(); ++i) {
        if (result[i] == '{' && result[i + 1] == '{') {
            return i;
        }
    }
    return result.length();
}

static bool advancePastQuotedChar(FunctionExpressionParams& params, const std::string& result, size_t& j) {
    if (result[j] != params.quoteChar) {
        return false;
    }
    if (j + 1 < result.length() && result[j + 1] == params.quoteChar) {
        ++j;
    } else {
        params.inQuotes = false;
    }
    return true;
}

static bool processOneExpression(std::string& result, std::function<std::string(const std::string&)>& parseFunctionExpr) {
    size_t i = findNextExpressionStart(result, 0);
    while (i + 2 < result.length()) {
        size_t startPos = i;
        size_t funcStart = 0;
        FunctionExpressionParams params;
        size_t j = i + 2;
        while (j + 1 < result.length()) {
            if (!params.inQuotes) {
                ProcessUnquotedContext ctx(result, j, i, startPos, funcStart, parseFunctionExpr);
                UnquotedAction action = handleUnquotedCharInTemplate(params, ctx);
                if (action == UnquotedAction::ProcessedAndBreak) {
                    return true;
                }
                if (action == UnquotedAction::Break) {
                    break;
                }
            } else if (advancePastQuotedChar(params, result, j)) {
                // quoted char handled
            }
            ++j;
        }
        i = findNextExpressionStart(result, i + 1);
    }
    return false;
}

void CLIFunctions::processFunctionExpressions(std::string& result, std::function<std::string(const std::string&)> parseFunctionExpr) {
    const int maxIterations = 100;
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        if (!processOneExpression(result, parseFunctionExpr)) {
            break;
        }
    }
}

} // namespace TemplateBuilder
