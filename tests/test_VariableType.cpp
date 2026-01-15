#include <gtest/gtest.h>
#include "../src/types/VariableType.hpp"
#include <stdexcept>

using namespace TemplateBuilder;

class VariableTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(VariableTypeTest, DefaultConstructor) {
    Variable var;
    EXPECT_EQ(var.getName(), "");
    EXPECT_EQ(var.getType(), VariableType::vtString);
    EXPECT_FALSE(var.hasValue());
}

TEST_F(VariableTypeTest, ConstructorWithNameAndType) {
    Variable var("testVar", VariableType::vtString);
    EXPECT_EQ(var.getName(), "testVar");
    EXPECT_EQ(var.getType(), VariableType::vtString);
    EXPECT_FALSE(var.hasValue());
}

TEST_F(VariableTypeTest, ConstructorWithNameTypeAndValue) {
    Variable var("testVar", VariableType::vtString, "testValue");
    EXPECT_EQ(var.getName(), "testVar");
    EXPECT_EQ(var.getType(), VariableType::vtString);
    EXPECT_TRUE(var.hasValue());
    EXPECT_EQ(var.getValue(), "testValue");
}

TEST_F(VariableTypeTest, SetterAndGetter) {
    Variable var;
    
    var.setName("myVariable");
    EXPECT_EQ(var.getName(), "myVariable");
    
    var.setType(VariableType::vtString);
    EXPECT_EQ(var.getType(), VariableType::vtString);
    
    var.setValue("myValue");
    EXPECT_TRUE(var.hasValue());
    EXPECT_EQ(var.getValue(), "myValue");
}

TEST_F(VariableTypeTest, ClearValue) {
    Variable var("testVar", VariableType::vtString, "testValue");
    EXPECT_TRUE(var.hasValue());
    
    var.clearValue();
    EXPECT_FALSE(var.hasValue());
    EXPECT_THROW(var.getValue(), std::runtime_error);
}

TEST_F(VariableTypeTest, GetValueWithoutSetting) {
    Variable var("testVar", VariableType::vtString);
    EXPECT_THROW(var.getValue(), std::runtime_error);
}

TEST_F(VariableTypeTest, StringToTypeValid) {
    EXPECT_EQ(Variable::stringToType("string"), VariableType::vtString);
    EXPECT_EQ(Variable::stringToType("String"), VariableType::vtString);
    EXPECT_EQ(Variable::stringToType("STRING"), VariableType::vtString);
    EXPECT_EQ(Variable::stringToType("StRiNg"), VariableType::vtString);
}

TEST_F(VariableTypeTest, StringToTypeInvalid) {
    EXPECT_THROW(Variable::stringToType("invalid"), std::invalid_argument);
    EXPECT_THROW(Variable::stringToType("number"), std::invalid_argument);
    EXPECT_THROW(Variable::stringToType(""), std::invalid_argument);
}
