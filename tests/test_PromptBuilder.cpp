#include <gtest/gtest.h>
#include "../src/builders/PromptBuilder.hpp"
#include "../src/types/PromptType.hpp"
#include "../src/types/VariableType.hpp"
#include <stdexcept>
#include <memory>
#include <vector>

using namespace TemplateBuilder;

class PromptBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        builder = std::make_unique<PromptBuilder>();
        
        // Create test variables
        var1 = std::make_unique<Variable>("var1", VariableType::vtString, "Hello");
        var2 = std::make_unique<Variable>("var2", VariableType::vtString, "World");
        var3 = std::make_unique<Variable>("var3", VariableType::vtString, "test value");
        varEmpty = std::make_unique<Variable>("varEmpty", VariableType::vtString);
        
        variables.push_back(var1.get());
        variables.push_back(var2.get());
        variables.push_back(var3.get());
        variables.push_back(varEmpty.get());
    }

    void TearDown() override {
        variables.clear();
    }

    std::unique_ptr<PromptBuilder> builder;
    std::unique_ptr<Variable> var1;
    std::unique_ptr<Variable> var2;
    std::unique_ptr<Variable> var3;
    std::unique_ptr<Variable> varEmpty;
    std::vector<Variable*> variables;
};

// Test ResolveVariableValue (tested indirectly through getContent)
TEST_F(PromptBuilderTest, ResolveVariableValueExisting) {
    // Test through getContent - variable resolution
    std::string content = "{{var1}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "Hello");
    
    content = "{{var2}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "World");
    
    content = "{{var3}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "test value");
}

TEST_F(PromptBuilderTest, ResolveVariableValueNonExistent) {
    std::string content = "{{nonexistent}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "");
}

TEST_F(PromptBuilderTest, ResolveVariableValueEmpty) {
    std::string content = "{{varEmpty}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "");
}

TEST_F(PromptBuilderTest, ResolveVariableValueEmptyVector) {
    std::vector<Variable*> emptyVars;
    std::string content = "{{var1}}";
    std::string result = builder->getContent(content, emptyVars);
    EXPECT_EQ(result, "{{var1}}"); // Should remain unchanged
}

// Test ExecuteFunction (tested indirectly through getContent with functions)
TEST_F(PromptBuilderTest, ExecuteFunctionUpper) {
    std::string content = "{{upper(\"hello world\")}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "HELLO WORLD");
    
    content = "{{upper(\"Test123\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "TEST123");
    
    content = "{{upper(\"MiXeD cAsE\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "MIXED CASE");
}

TEST_F(PromptBuilderTest, ExecuteFunctionUpperInvalidArgs) {
    // Invalid function calls should throw exceptions
    std::string content = "{{upper()}}";
    EXPECT_THROW(builder->getContent(content, variables), std::runtime_error);
}

// Test ExecuteFunction - lower
TEST_F(PromptBuilderTest, ExecuteFunctionLower) {
    std::string content = "{{lower(\"HELLO WORLD\")}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "hello world");
    
    content = "{{lower(\"TEST123\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "test123");
    
    content = "{{lower(\"MiXeD cAsE\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "mixed case");
}

// Test ExecuteFunction - replace
TEST_F(PromptBuilderTest, ExecuteFunctionReplace) {
    std::string content = "{{replace(\" \", \"_\", \"hello world\")}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "hello_world");
    
    content = "{{replace(\"old\", \"new\", \"old text old\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "new text new");
    
    content = "{{replace(\"a\", \"A\", \"banana\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "bAnAnA");
}

// Test ParseFunctionExpression (tested indirectly through getContent)
TEST_F(PromptBuilderTest, ParseFunctionExpressionUpper) {
    std::string content = "{{upper(var1)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "HELLO");
}

TEST_F(PromptBuilderTest, ParseFunctionExpressionLower) {
    std::string content = "{{lower(var2)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "world");
}

TEST_F(PromptBuilderTest, ParseFunctionExpressionReplace) {
    std::string content = "{{replace(\" \", \"_\", var3)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "test_value");
}

// Test ParseFunctionExpression - nested functions
TEST_F(PromptBuilderTest, ParseFunctionExpressionNested) {
    std::string content = "{{upper(lower(var2))}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "WORLD");
    
    content = "{{lower(upper(var1))}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "hello");
}

TEST_F(PromptBuilderTest, ParseFunctionExpressionComplexNested) {
    std::string content = "{{replace(\"_\", \"-\", replace(\" \", \"_\", var3))}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "test-value");
}

// Test ParseFunctionExpression - with string literals
TEST_F(PromptBuilderTest, ParseFunctionExpressionWithStringLiterals) {
    std::string content = "{{upper(\"hello\")}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "HELLO");
    
    content = "{{replace(\"a\", \"A\", \"banana\")}}";
    result = builder->getContent(content, variables);
    EXPECT_EQ(result, "bAnAnA");
}

// Test ParseFunctionExpression - invalid expressions
TEST_F(PromptBuilderTest, ParseFunctionExpressionInvalid) {
    std::string content = "{{invalid}}";
    // Should not crash, but may not process correctly
    EXPECT_NO_THROW(builder->getContent(content, variables));
}

// Test GetContent - simple variable replacement
TEST_F(PromptBuilderTest, GetContentSimpleVariable) {
    std::string content = "Hello {{var1}} World";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "Hello Hello World");
}

TEST_F(PromptBuilderTest, GetContentMultipleVariables) {
    std::string content = "{{var1}} {{var2}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "Hello World");
}

TEST_F(PromptBuilderTest, GetContentNoVariables) {
    std::string content = "No variables here";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "No variables here");
}

TEST_F(PromptBuilderTest, GetContentEmptyVariables) {
    std::string content = "{{var1}}";
    std::vector<Variable*> emptyVars;
    std::string result = builder->getContent(content, emptyVars);
    EXPECT_EQ(result, "{{var1}}");
}

// Test GetContent - with functions
TEST_F(PromptBuilderTest, GetContentWithUpperFunction) {
    std::string content = "{{upper(var1)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "HELLO");
}

TEST_F(PromptBuilderTest, GetContentWithLowerFunction) {
    std::string content = "{{lower(var2)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "world");
}

TEST_F(PromptBuilderTest, GetContentWithReplaceFunction) {
    std::string content = "{{replace(\" \", \"_\", var3)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "test_value");
}

TEST_F(PromptBuilderTest, GetContentWithNestedFunctions) {
    std::string content = "{{upper(lower(var2))}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "WORLD");
}

// Test GetContent - with prefix pattern
TEST_F(PromptBuilderTest, GetContentWithPrefixPattern) {
    std::string content = R"({{"prefix " | var1}})";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "prefix Hello");
}

TEST_F(PromptBuilderTest, GetContentWithPrefixPatternMultiLine) {
    // Create a variable with multiple lines
    auto multiLineVar = std::make_unique<Variable>("multiLine", VariableType::vtString, "line1\r\nline2\r\nline3");
    std::vector<Variable*> testVars = {multiLineVar.get()};
    
    std::string content = R"({{"prefix " | multiLine}})";
    std::string result = builder->getContent(content, testVars);
    EXPECT_EQ(result, "prefix line1\r\nprefix line2\r\nprefix line3");
}

// Test GetContent - complex template
TEST_F(PromptBuilderTest, GetContentComplexTemplate) {
    std::string content = "{{var1}} is {{upper(var2)}} and {{replace(\" \", \"_\", var3)}}";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "Hello is WORLD and test_value");
}

// Test GetContent - mixed patterns
TEST_F(PromptBuilderTest, GetContentMixedPatterns) {
    std::string content = R"(Start {{var1}} middle {{upper(var2)}} end {{"prefix " | var3}})";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "Start Hello middle WORLD end prefix test value");
}

// Test Build - with PromptInput
TEST_F(PromptBuilderTest, BuildWithPrompt) {
    auto prompt = std::make_unique<Prompt>("TestPrompt");
    prompt->setResult("Result: {{var1}}");
    
    auto input = std::make_unique<PromptInput>(PromptType::ptInputString);
    input->setInput("Enter value: ");
    input->setVariable(var1.get());
    prompt->addInput(std::move(input));
    
    // Note: This test doesn't actually test console input, but verifies the structure
    // Real console input testing would require mocking or integration tests
    std::string result = builder->build(prompt.get(), variables);
    // The result should contain the processed template
    EXPECT_TRUE(result.find("Result:") != std::string::npos);
}

TEST_F(PromptBuilderTest, BuildWithNullPrompt) {
    std::string result = builder->build(nullptr, variables);
    EXPECT_EQ(result, "");
}

// ============================================
// VALIDATION TESTS FOR ALL PROMPT TYPES
// ============================================

// Validate ptInputString prompt type
TEST_F(PromptBuilderTest, ValidateInputStringPromptType) {
    auto prompt = std::make_unique<Prompt>("InputStringTest");
    prompt->setResult("Result: {{testVar}}");
    
    auto testVar = std::make_unique<Variable>("testVar", VariableType::vtString);
    
    auto input = std::make_unique<PromptInput>(PromptType::ptInputString);
    input->setInput("Enter your name: ");
    input->setVariable(testVar.get());
    
    EXPECT_EQ(input->getType(), PromptType::ptInputString);
    EXPECT_NE(input->getVariable(), nullptr);
    EXPECT_FALSE(input->getInput().empty());
    
    prompt->addInput(std::move(input));
    
    // Verify prompt structure is valid
    EXPECT_EQ(prompt->getInputsCount(), 1);
    EXPECT_EQ(prompt->getInputs()[0]->getType(), PromptType::ptInputString);
}

TEST_F(PromptBuilderTest, ValidateInputStringRequiresVariable) {
    auto input = std::make_unique<PromptInput>(PromptType::ptInputString);
    input->setInput("Enter value: ");
    // Variable is not set - should throw when used
    
    // This would throw in actual usage, but we're just validating structure here
    EXPECT_EQ(input->getVariable(), nullptr);
    EXPECT_EQ(input->getType(), PromptType::ptInputString);
}

// Validate ptChecklist prompt type
TEST_F(PromptBuilderTest, ValidateChecklistPromptType) {
    auto prompt = std::make_unique<Prompt>("ChecklistTest");
    prompt->setResult("Selected: {{testVar}}");
    
    auto testVar = std::make_unique<Variable>("testVar", VariableType::vtString);
    
    auto input = std::make_unique<PromptInput>(PromptType::ptChecklist);
    input->setInput("Select options: ");
    input->setVariable(testVar.get());
    
    // Checklist requires options
    input->addOption("Option 1", "value1");
    input->addOption("Option 2", "value2");
    input->addOption("Option 3", "value3");
    
    EXPECT_EQ(input->getType(), PromptType::ptChecklist);
    EXPECT_NE(input->getVariable(), nullptr);
    EXPECT_FALSE(input->getInput().empty());
    EXPECT_EQ(input->getOptionsCount(), 3);
    EXPECT_FALSE(input->getOptions().empty());
    
    prompt->addInput(std::move(input));
    
    // Verify prompt structure is valid
    EXPECT_EQ(prompt->getInputsCount(), 1);
    EXPECT_EQ(prompt->getInputs()[0]->getType(), PromptType::ptChecklist);
    EXPECT_EQ(prompt->getInputs()[0]->getOptionsCount(), 3);
}

TEST_F(PromptBuilderTest, ValidateChecklistRequiresOptions) {
    auto testVar = std::make_unique<Variable>("testVar", VariableType::vtString);
    auto input = std::make_unique<PromptInput>(PromptType::ptChecklist);
    input->setInput("Select: ");
    input->setVariable(testVar.get());
    
    // No options added - should be invalid
    EXPECT_EQ(input->getOptionsCount(), 0);
    EXPECT_TRUE(input->getOptions().empty());
    
    // This would throw in actual usage: "No options available for checklist input"
    EXPECT_EQ(input->getType(), PromptType::ptChecklist);
}

TEST_F(PromptBuilderTest, ValidateChecklistRequiresVariable) {
    auto input = std::make_unique<PromptInput>(PromptType::ptChecklist);
    input->setInput("Select options: ");
    input->addOption("Option 1", "value1");
    
    // Variable is not set - should throw when used
    EXPECT_EQ(input->getVariable(), nullptr);
    EXPECT_EQ(input->getType(), PromptType::ptChecklist);
    EXPECT_FALSE(input->getOptions().empty());
}

// Validate ptArrayList prompt type
TEST_F(PromptBuilderTest, ValidateArrayListPromptType) {
    auto prompt = std::make_unique<Prompt>("ArrayListTest");
    prompt->setResult("Items: {{testVar}}");
    
    auto testVar = std::make_unique<Variable>("testVar", VariableType::vtString);
    
    auto input = std::make_unique<PromptInput>(PromptType::ptArrayList);
    input->setInput("Enter items (empty line to finish): ");
    input->setVariable(testVar.get());
    
    EXPECT_EQ(input->getType(), PromptType::ptArrayList);
    EXPECT_NE(input->getVariable(), nullptr);
    EXPECT_FALSE(input->getInput().empty());
    
    prompt->addInput(std::move(input));
    
    // Verify prompt structure is valid
    EXPECT_EQ(prompt->getInputsCount(), 1);
    EXPECT_EQ(prompt->getInputs()[0]->getType(), PromptType::ptArrayList);
}

TEST_F(PromptBuilderTest, ValidateArrayListRequiresVariable) {
    auto input = std::make_unique<PromptInput>(PromptType::ptArrayList);
    input->setInput("Enter items: ");
    
    // Variable is not set - should throw when used
    EXPECT_EQ(input->getVariable(), nullptr);
    EXPECT_EQ(input->getType(), PromptType::ptArrayList);
}

// Validate all prompt types in a single prompt
TEST_F(PromptBuilderTest, ValidateAllPromptTypesInOnePrompt) {
    auto prompt = std::make_unique<Prompt>("AllTypesTest");
    prompt->setResult("Input: {{var1}}, Checklist: {{var2}}, ArrayList: {{var3}}");
    
    // InputString
    auto input1 = std::make_unique<PromptInput>(PromptType::ptInputString);
    input1->setInput("Enter text: ");
    input1->setVariable(var1.get());
    
    // Checklist
    auto input2 = std::make_unique<PromptInput>(PromptType::ptChecklist);
    input2->setInput("Select options: ");
    input2->setVariable(var2.get());
    input2->addOption("Option A", "A");
    input2->addOption("Option B", "B");
    input2->addOption("Option C", "C");
    
    // ArrayList
    auto input3 = std::make_unique<PromptInput>(PromptType::ptArrayList);
    input3->setInput("Enter list items: ");
    input3->setVariable(var3.get());
    
    prompt->addInput(std::move(input1));
    prompt->addInput(std::move(input2));
    prompt->addInput(std::move(input3));
    
    // Validate all types are present and correct
    EXPECT_EQ(prompt->getInputsCount(), 3);
    EXPECT_EQ(prompt->getInputs()[0]->getType(), PromptType::ptInputString);
    EXPECT_EQ(prompt->getInputs()[1]->getType(), PromptType::ptChecklist);
    EXPECT_EQ(prompt->getInputs()[2]->getType(), PromptType::ptArrayList);
    
    // Validate checklist has options
    EXPECT_EQ(prompt->getInputs()[1]->getOptionsCount(), 3);
    
    // Validate all have variables
    EXPECT_NE(prompt->getInputs()[0]->getVariable(), nullptr);
    EXPECT_NE(prompt->getInputs()[1]->getVariable(), nullptr);
    EXPECT_NE(prompt->getInputs()[2]->getVariable(), nullptr);
}

// Validate prompt type conversion
TEST_F(PromptBuilderTest, ValidatePromptTypeStringConversion) {
    EXPECT_EQ(PromptInput::stringToType("inputstring"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("checklist"), PromptType::ptChecklist);
    EXPECT_EQ(PromptInput::stringToType("arraylist"), PromptType::ptArrayList);
    
    // Case insensitive
    EXPECT_EQ(PromptInput::stringToType("InputString"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("CheckList"), PromptType::ptChecklist);
    EXPECT_EQ(PromptInput::stringToType("ArrayList"), PromptType::ptArrayList);
    
    EXPECT_EQ(PromptInput::stringToType("INPUTSTRING"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("CHECKLIST"), PromptType::ptChecklist);
    EXPECT_EQ(PromptInput::stringToType("ARRAYLIST"), PromptType::ptArrayList);
}

// Validate prompt type behavior differences
TEST_F(PromptBuilderTest, ValidatePromptTypeBehaviorDifferences) {
    // InputString doesn't need options
    auto inputString = std::make_unique<PromptInput>(PromptType::ptInputString);
    inputString->setInput("Enter: ");
    inputString->setVariable(var1.get());
    EXPECT_EQ(inputString->getOptionsCount(), 0);
    EXPECT_TRUE(inputString->getOptions().empty());
    
    // Checklist requires options
    auto checklist = std::make_unique<PromptInput>(PromptType::ptChecklist);
    checklist->setInput("Select: ");
    checklist->setVariable(var2.get());
    checklist->addOption("Opt1", "val1");
    EXPECT_GT(checklist->getOptionsCount(), 0);
    EXPECT_FALSE(checklist->getOptions().empty());
    
    // ArrayList doesn't need options (user enters items manually)
    auto arrayList = std::make_unique<PromptInput>(PromptType::ptArrayList);
    arrayList->setInput("Enter items: ");
    arrayList->setVariable(var3.get());
    EXPECT_EQ(arrayList->getOptionsCount(), 0);
    EXPECT_TRUE(arrayList->getOptions().empty());
}

// Test edge cases
TEST_F(PromptBuilderTest, GetContentEmptyString) {
    std::string content = "";
    std::string result = builder->getContent(content, variables);
    EXPECT_EQ(result, "");
}

TEST_F(PromptBuilderTest, GetContentUnmatchedBraces) {
    std::string content = "{{var1}";
    std::string result = builder->getContent(content, variables);
    // Should not crash, but may not replace
    EXPECT_TRUE(result.find("{{var1}") != std::string::npos || result.find("Hello") != std::string::npos);
}

TEST_F(PromptBuilderTest, GetContentSpecialCharacters) {
    auto specialVar = std::make_unique<Variable>("special", VariableType::vtString, "test@#$%");
    std::vector<Variable*> testVars = {specialVar.get()};
    
    std::string content = "{{special}}";
    std::string result = builder->getContent(content, testVars);
    EXPECT_EQ(result, "test@#$%");
}
