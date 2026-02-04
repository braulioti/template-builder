#include "cli-utils/CLIFunctions.hpp"
#include <algorithm>
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

std::vector<std::string> parseArgumentsFromString(const std::string& argsString) {
    std::vector<std::string> parsedArguments;
    if (argsString.empty()) {
        return parsedArguments;
    }
    size_t startPos = 0;
    bool inQuotes = false;
    char quoteChar = '\0';
    int functionDepth = 0;
    for (size_t j = 0; j < argsString.length(); ++j) {
        char currentChar = argsString[j];
        if (!inQuotes) {
            if (currentChar == '"' || currentChar == '\'') {
                inQuotes = true;
                quoteChar = currentChar;
            } else if (currentChar == '(') {
                ++functionDepth;
            } else if (currentChar == ')') {
                --functionDepth;
            } else if (currentChar == ',' && functionDepth == 0) {
                std::string arg = trimArg(argsString.substr(startPos, j - startPos));
                if (!arg.empty()) {
                    parsedArguments.push_back(arg);
                }
                startPos = j + 1;
            }
        } else if (currentChar == quoteChar) {
            if (j + 1 < argsString.length() && argsString[j + 1] == quoteChar) {
                ++j;
            } else {
                inQuotes = false;
            }
        }
    }
    if (startPos < argsString.length()) {
        std::string lastArg = trimArg(argsString.substr(startPos));
        if (!lastArg.empty()) {
            parsedArguments.push_back(lastArg);
        }
    }
    return parsedArguments;
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
    size_t argCount = arguments.size();

    // Convert function name to lowercase for comparison
    std::string lowerFunctionName = functionName;
    std::transform(lowerFunctionName.begin(), lowerFunctionName.end(), lowerFunctionName.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lowerFunctionName == "upper") {
        if (argCount != 1) {
            throw std::runtime_error("Function 'upper' expects 1 argument, got " + std::to_string(argCount));
        }
        std::string result = arguments[0];
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return result;
    } else if (lowerFunctionName == "lower") {
        if (argCount != 1) {
            throw std::runtime_error("Function 'lower' expects 1 argument, got " + std::to_string(argCount));
        }
        std::string result = arguments[0];
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return result;
    } else if (lowerFunctionName == "replace") {
        if (argCount != 3) {
            throw std::runtime_error("Function 'replace' expects 3 arguments, got " + std::to_string(argCount));
        }
        std::string result = arguments[2];
        std::string search = arguments[0];
        std::string replacement = arguments[1];

        size_t pos = 0;
        while ((pos = result.find(search, pos)) != std::string::npos) {
            result.replace(pos, search.length(), replacement);
            pos += replacement.length();
        }
        return result;
    } else {
        throw std::runtime_error("Unknown function: " + functionName);
    }
}

void CLIFunctions::processFunctionExpressions(std::string& result, std::function<std::string(const std::string&)> parseFunctionExpr) {
    const int maxIterations = 100;
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        bool processed = false;
        for (size_t i = 0; i + 2 < result.length(); ++i) {
            if (result[i] != '{' || result[i + 1] != '{') {
                continue;
            }
            size_t startPos = i;
            size_t funcStart = 0;
            int parenDepth = 0;
            bool inQuotes = false;
            char quoteChar = '\0';
            bool foundFunc = false;
            size_t j = i + 2;
            while (j + 1 < result.length()) {
                if (!inQuotes) {
                    if (result[j] == '"' || result[j] == '\'') {
                        inQuotes = true;
                        quoteChar = result[j];
                    } else if (result[j] == '(') {
                        if (parenDepth == 0) {
                            funcStart = i + 2;
                            foundFunc = true;
                        }
                        ++parenDepth;
                    } else if (result[j] == ')') {
                        --parenDepth;
                        if (parenDepth == 0 && foundFunc && j + 2 < result.length() && result[j + 1] == '}' && result[j + 2] == '}') {
                            std::string functionExpression = result.substr(funcStart, j - funcStart + 1);
                            std::string functionResult = parseFunctionExpr(functionExpression);
                            result.replace(startPos, j + 3 - startPos, functionResult);
                            processed = true;
                            break;
                        }
                    } else if (j + 1 < result.length() && result[j] == '}' && result[j + 1] == '}' && !foundFunc) {
                        break;
                    }
                } else if (result[j] == quoteChar) {
                    if (j + 1 < result.length() && result[j + 1] == quoteChar) {
                        ++j;
                    } else {
                        inQuotes = false;
                    }
                }
                ++j;
            }
            if (processed) {
                break;
            }
        }
        if (!processed) {
            break;
        }
    }
}

} // namespace TemplateBuilder
