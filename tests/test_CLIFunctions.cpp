#include <gtest/gtest.h>
#include "cli-utils/CLIFunctions.hpp"
#include "types/VariableType.hpp"
#include <stdexcept>
#include <memory>
#include <vector>

using namespace TemplateBuilder;

class CLIFunctionsTest : public ::testing::Test {
protected:
    void SetUp() override {
        var1 = std::make_unique<Variable>("var1", VariableType::vtString, "Hello");
        var2 = std::make_unique<Variable>("var2", VariableType::vtString, "World");
        var3 = std::make_unique<Variable>("var3", VariableType::vtString, "test value");
        variables.push_back(var1.get());
        variables.push_back(var2.get());
        variables.push_back(var3.get());
    }

    std::string resolveVariable(const std::string& name) {
        for (Variable* v : variables) {
            if (v && v->getName() == name && v->hasValue()) {
                return v->getValue();
            }
        }
        return "";
    }

    std::unique_ptr<Variable> var1;
    std::unique_ptr<Variable> var2;
    std::unique_ptr<Variable> var3;
    std::vector<Variable*> variables;
};

// Test executeFunction - upper
TEST_F(CLIFunctionsTest, ExecuteFunctionUpper) {
    std::vector<std::string> args = {"hello world"};
    EXPECT_EQ(CLIFunctions::executeFunction("upper", args), "HELLO WORLD");

    args = {"Test123"};
    EXPECT_EQ(CLIFunctions::executeFunction("upper", args), "TEST123");

    args = {"MiXeD cAsE"};
    EXPECT_EQ(CLIFunctions::executeFunction("upper", args), "MIXED CASE");
}

// Test executeFunction - lower
TEST_F(CLIFunctionsTest, ExecuteFunctionLower) {
    std::vector<std::string> args = {"HELLO WORLD"};
    EXPECT_EQ(CLIFunctions::executeFunction("lower", args), "hello world");

    args = {"TEST123"};
    EXPECT_EQ(CLIFunctions::executeFunction("lower", args), "test123");
}

// Test executeFunction - replace
TEST_F(CLIFunctionsTest, ExecuteFunctionReplace) {
    std::vector<std::string> args = {" ", "_", "hello world"};
    EXPECT_EQ(CLIFunctions::executeFunction("replace", args), "hello_world");

    args = {"old", "new", "old text old"};
    EXPECT_EQ(CLIFunctions::executeFunction("replace", args), "new text new");

    args = {"a", "A", "banana"};
    EXPECT_EQ(CLIFunctions::executeFunction("replace", args), "bAnAnA");
}

// Test executeFunction - invalid
TEST_F(CLIFunctionsTest, ExecuteFunctionInvalid) {
    std::vector<std::string> args = {};
    EXPECT_THROW(CLIFunctions::executeFunction("upper", args), std::runtime_error);

    args = {"a", "b"};
    EXPECT_THROW(CLIFunctions::executeFunction("upper", args), std::runtime_error);

    args = {"x"};
    EXPECT_THROW(CLIFunctions::executeFunction("unknown", args), std::runtime_error);
}

// Test executeFunction - case insensitive
TEST_F(CLIFunctionsTest, ExecuteFunctionCaseInsensitive) {
    std::vector<std::string> args = {"hello"};
    EXPECT_EQ(CLIFunctions::executeFunction("UPPER", args), "HELLO");
    EXPECT_EQ(CLIFunctions::executeFunction("Upper", args), "HELLO");
}

// Test parseFunctionExpression - with variable resolution
TEST_F(CLIFunctionsTest, ParseFunctionExpressionWithVariable) {
    auto resolve = [this](const std::string& name) { return resolveVariable(name); };
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("upper(var1)", resolve), "HELLO");
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("lower(var2)", resolve), "world");
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("replace(\" \", \"_\", var3)", resolve), "test_value");
}

// Test parseFunctionExpression - with string literals
TEST_F(CLIFunctionsTest, ParseFunctionExpressionWithStringLiterals) {
    auto resolve = [](const std::string&) { return ""; };
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("upper(\"hello\")", resolve), "HELLO");
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("lower(\"WORLD\")", resolve), "world");
}

// Test parseFunctionExpression - nested
TEST_F(CLIFunctionsTest, ParseFunctionExpressionNested) {
    auto resolve = [this](const std::string& name) { return resolveVariable(name); };
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("upper(lower(var2))", resolve), "WORLD");
    EXPECT_EQ(CLIFunctions::parseFunctionExpression("lower(upper(var1))", resolve), "hello");
}

// Test parseFunctionExpression - invalid
TEST_F(CLIFunctionsTest, ParseFunctionExpressionInvalid) {
    auto resolve = [](const std::string&) { return ""; };
    EXPECT_THROW(CLIFunctions::parseFunctionExpression("invalid", resolve), std::runtime_error);
    EXPECT_THROW(CLIFunctions::parseFunctionExpression("", resolve), std::runtime_error);
}

// Test processFunctionExpressions
TEST_F(CLIFunctionsTest, ProcessFunctionExpressions) {
    auto parseExpr = [this](const std::string& expr) {
        return CLIFunctions::parseFunctionExpression(expr, [this](const std::string& name) {
            return resolveVariable(name);
        });
    };

    std::string result = "{{upper(var1)}} {{lower(var2)}}";
    CLIFunctions::processFunctionExpressions(result, parseExpr);
    EXPECT_EQ(result, "HELLO world");

    result = "Text: {{replace(\" \", \"_\", var3)}}";
    CLIFunctions::processFunctionExpressions(result, parseExpr);
    EXPECT_EQ(result, "Text: test_value");
}
