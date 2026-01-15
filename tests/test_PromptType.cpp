#include <gtest/gtest.h>
#include "../src/types/PromptType.hpp"
#include "../src/types/VariableType.hpp"
#include <stdexcept>
#include <memory>

using namespace TemplateBuilder;

class PromptTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test variable
        testVariable = std::make_unique<Variable>("testVar", VariableType::vtString);
    }

    void TearDown() override {
        // Cleanup
    }

    std::unique_ptr<Variable> testVariable;
};

// PromptInputOption tests
TEST_F(PromptTypeTest, PromptInputOptionDefaultConstructor) {
    PromptInputOption option;
    EXPECT_EQ(option.getName(), "");
    EXPECT_EQ(option.getValue(), "");
}

TEST_F(PromptTypeTest, PromptInputOptionConstructorWithParams) {
    PromptInputOption option("Option1", "Value1");
    EXPECT_EQ(option.getName(), "Option1");
    EXPECT_EQ(option.getValue(), "Value1");
}

TEST_F(PromptTypeTest, PromptInputOptionSetters) {
    PromptInputOption option;
    option.setName("MyOption");
    option.setValue("MyValue");
    EXPECT_EQ(option.getName(), "MyOption");
    EXPECT_EQ(option.getValue(), "MyValue");
}

// PromptInput tests
TEST_F(PromptTypeTest, PromptInputDefaultConstructor) {
    PromptInput input;
    EXPECT_EQ(input.getVariable(), nullptr);
    EXPECT_EQ(input.getInput(), "");
    EXPECT_EQ(input.getType(), PromptType::ptInputString);
    EXPECT_EQ(input.getOptionsCount(), 0);
}

TEST_F(PromptTypeTest, PromptInputConstructorWithType) {
    PromptInput input(PromptType::ptChecklist);
    EXPECT_EQ(input.getType(), PromptType::ptChecklist);
    EXPECT_EQ(input.getOptionsCount(), 0);
}

TEST_F(PromptTypeTest, PromptInputSetters) {
    PromptInput input;
    input.setVariable(testVariable.get());
    input.setInput("Enter value: ");
    input.setType(PromptType::ptArrayList);
    
    EXPECT_EQ(input.getVariable(), testVariable.get());
    EXPECT_EQ(input.getInput(), "Enter value: ");
    EXPECT_EQ(input.getType(), PromptType::ptArrayList);
}

TEST_F(PromptTypeTest, PromptInputAddOption) {
    PromptInput input;
    auto option = std::make_unique<PromptInputOption>("Option1", "Value1");
    input.addOption(std::move(option));
    
    EXPECT_EQ(input.getOptionsCount(), 1);
    EXPECT_EQ(input.getOptions()[0]->getName(), "Option1");
    EXPECT_EQ(input.getOptions()[0]->getValue(), "Value1");
}

TEST_F(PromptTypeTest, PromptInputAddOptionWithParams) {
    PromptInput input;
    input.addOption("Option1", "Value1");
    input.addOption("Option2", "Value2");
    
    EXPECT_EQ(input.getOptionsCount(), 2);
    EXPECT_EQ(input.getOptions()[0]->getName(), "Option1");
    EXPECT_EQ(input.getOptions()[1]->getName(), "Option2");
}

TEST_F(PromptTypeTest, PromptInputClearOptions) {
    PromptInput input;
    input.addOption("Option1", "Value1");
    input.addOption("Option2", "Value2");
    EXPECT_EQ(input.getOptionsCount(), 2);
    
    input.clearOptions();
    EXPECT_EQ(input.getOptionsCount(), 0);
}

TEST_F(PromptTypeTest, PromptInputStringToTypeValid) {
    EXPECT_EQ(PromptInput::stringToType("inputstring"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("InputString"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("INPUTSTRING"), PromptType::ptInputString);
    EXPECT_EQ(PromptInput::stringToType("checklist"), PromptType::ptChecklist);
    EXPECT_EQ(PromptInput::stringToType("CheckList"), PromptType::ptChecklist);
    EXPECT_EQ(PromptInput::stringToType("arraylist"), PromptType::ptArrayList);
    EXPECT_EQ(PromptInput::stringToType("ArrayList"), PromptType::ptArrayList);
}

TEST_F(PromptTypeTest, PromptInputStringToTypeInvalid) {
    EXPECT_THROW(PromptInput::stringToType("invalid"), std::invalid_argument);
    EXPECT_THROW(PromptInput::stringToType(""), std::invalid_argument);
}

// Prompt tests
TEST_F(PromptTypeTest, PromptDefaultConstructor) {
    Prompt prompt;
    EXPECT_EQ(prompt.getName(), "");
    EXPECT_EQ(prompt.getResult(), "");
    EXPECT_EQ(prompt.getInputsCount(), 0);
}

TEST_F(PromptTypeTest, PromptConstructorWithName) {
    Prompt prompt("MyPrompt");
    EXPECT_EQ(prompt.getName(), "MyPrompt");
    EXPECT_EQ(prompt.getResult(), "");
    EXPECT_EQ(prompt.getInputsCount(), 0);
}

TEST_F(PromptTypeTest, PromptSetters) {
    Prompt prompt;
    prompt.setName("TestPrompt");
    prompt.setResult("Result content");
    
    EXPECT_EQ(prompt.getName(), "TestPrompt");
    EXPECT_EQ(prompt.getResult(), "Result content");
}

TEST_F(PromptTypeTest, PromptAddInput) {
    Prompt prompt;
    auto input = std::make_unique<PromptInput>(PromptType::ptInputString);
    input->setInput("Enter name: ");
    prompt.addInput(std::move(input));
    
    EXPECT_EQ(prompt.getInputsCount(), 1);
    EXPECT_EQ(prompt.getInputs()[0]->getInput(), "Enter name: ");
    EXPECT_EQ(prompt.getInputs()[0]->getType(), PromptType::ptInputString);
}

TEST_F(PromptTypeTest, PromptClearInputs) {
    Prompt prompt;
    auto input1 = std::make_unique<PromptInput>(PromptType::ptInputString);
    auto input2 = std::make_unique<PromptInput>(PromptType::ptChecklist);
    prompt.addInput(std::move(input1));
    prompt.addInput(std::move(input2));
    EXPECT_EQ(prompt.getInputsCount(), 2);
    
    prompt.clearInputs();
    EXPECT_EQ(prompt.getInputsCount(), 0);
}

TEST_F(PromptTypeTest, PromptWithMultipleInputs) {
    Prompt prompt("ComplexPrompt");
    prompt.setResult("Result template");
    
    auto input1 = std::make_unique<PromptInput>(PromptType::ptInputString);
    input1->setInput("Enter name: ");
    input1->setVariable(testVariable.get());
    prompt.addInput(std::move(input1));
    
    auto input2 = std::make_unique<PromptInput>(PromptType::ptChecklist);
    input2->setInput("Select options: ");
    input2->addOption("Option1", "Value1");
    input2->addOption("Option2", "Value2");
    prompt.addInput(std::move(input2));
    
    EXPECT_EQ(prompt.getInputsCount(), 2);
    EXPECT_EQ(prompt.getInputs()[0]->getType(), PromptType::ptInputString);
    EXPECT_EQ(prompt.getInputs()[1]->getType(), PromptType::ptChecklist);
    EXPECT_EQ(prompt.getInputs()[1]->getOptionsCount(), 2);
}
