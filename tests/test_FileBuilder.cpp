#include <gtest/gtest.h>
#include "../src/builders/FileBuilder.hpp"
#include "../src/types/FileType.hpp"
#include "../src/types/PromptType.hpp"
#include "../src/types/VariableType.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

using namespace TemplateBuilder;

class FileBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir = std::filesystem::temp_directory_path() / "template_builder_test";
        std::filesystem::create_directories(testDir);
        originalDir = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
        
        // Create test variables
        testVar1 = std::make_unique<Variable>("var1", VariableType::vtString, "Hello");
        testVar2 = std::make_unique<Variable>("var2", VariableType::vtString, "World");
        testVariables.push_back(testVar1.get());
        testVariables.push_back(testVar2.get());
        
        // Create test prompt
        testPrompt = std::make_unique<Prompt>("testPrompt");
        testPrompt->setResult("Result: {{var1}} {{var2}}");
        
        builder = std::make_unique<FileBuilder>();
    }

    void TearDown() override {
        // Restore original directory
        std::filesystem::current_path(originalDir);
        
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::filesystem::path testDir;
    std::filesystem::path originalDir;
    std::unique_ptr<FileBuilder> builder;
    std::unique_ptr<Variable> testVar1;
    std::unique_ptr<Variable> testVar2;
    std::vector<Variable*> testVariables;
    std::unique_ptr<Prompt> testPrompt;
};

// Test file creation with static content
TEST_F(FileBuilderTest, BuildFileWithStaticContent) {
    auto fileData = std::make_unique<FileData>("output/test.txt", "Static content");
    fileData->setVariables(&testVariables);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "test.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Static content");
}

// Test file creation with variable substitution
TEST_F(FileBuilderTest, BuildFileWithVariableSubstitution) {
    auto fileData = std::make_unique<FileData>("output/test.txt", "{{var1}} {{var2}}");
    fileData->setVariables(&testVariables);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "test.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Hello World");
}

// Test file creation with prompt
TEST_F(FileBuilderTest, BuildFileWithPrompt) {
    auto fileData = std::make_unique<FileData>("output/prompted.txt", "");
    fileData->setPrompt(testPrompt.get());
    fileData->setVariables(&testVariables);
    
    // Note: This test assumes prompt builder processes variables correctly
    // The actual prompt interaction would require mocking console input
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "prompted.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
}

// Test automatic parent directory creation
TEST_F(FileBuilderTest, ValidateAutomaticParentDirectoryCreation) {
    // Create file in nested directory structure
    auto fileData = std::make_unique<FileData>("level1/level2/level3/test.txt", "Content");
    
    std::filesystem::path parentDir = testDir / "level1" / "level2" / "level3";
    
    // Directory should not exist before build
    EXPECT_FALSE(std::filesystem::exists(parentDir));
    
    builder->build(fileData.get());
    
    // Directory should be created automatically
    EXPECT_TRUE(std::filesystem::exists(parentDir));
    
    // File should exist
    std::filesystem::path filePath = parentDir / "test.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
}

// Test automatic parent directory creation - multiple levels
TEST_F(FileBuilderTest, ValidateAutomaticParentDirectoryCreationMultipleLevels) {
    auto fileData = std::make_unique<FileData>("a/b/c/d/e/f/g/deep.txt", "Deep content");
    
    std::filesystem::path deepDir = testDir / "a" / "b" / "c" / "d" / "e" / "f" / "g";
    
    EXPECT_FALSE(std::filesystem::exists(deepDir));
    builder->build(fileData.get());
    EXPECT_TRUE(std::filesystem::exists(deepDir));
    
    std::filesystem::path filePath = deepDir / "deep.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Deep content");
}

// Test file in root directory (no parent directory needed)
TEST_F(FileBuilderTest, BuildFileInRootDirectory) {
    auto fileData = std::make_unique<FileData>("rootfile.txt", "Root content");
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "rootfile.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "Root content");
}

// Test empty path throws exception
TEST_F(FileBuilderTest, EmptyPathThrowsException) {
    auto fileData = std::make_unique<FileData>("", "Content");
    
    EXPECT_THROW(builder->build(fileData.get()), std::runtime_error);
}

// Test null file (should not crash)
TEST_F(FileBuilderTest, BuildWithNullFile) {
    EXPECT_NO_THROW(builder->build(nullptr));
}

// Test UTF-8 encoding preservation
TEST_F(FileBuilderTest, UTF8EncodingPreservation) {
    std::string utf8Content = "Test with UTF-8: áéíóú àèìòù çñ üö";
    auto fileData = std::make_unique<FileData>("output/utf8.txt", utf8Content);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "utf8.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, utf8Content);
}

// Test UTF-8 with special characters
TEST_F(FileBuilderTest, UTF8SpecialCharacters) {
    std::string specialContent = "特殊字符: 中文 日本語 한국어 Русский العربية";
    auto fileData = std::make_unique<FileData>("output/special.txt", specialContent);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "special.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, specialContent);
}

// Test file with template functions
TEST_F(FileBuilderTest, BuildFileWithTemplateFunctions) {
    auto fileData = std::make_unique<FileData>("output/functions.txt", "{{upper(var1)}} {{lower(var2)}}");
    fileData->setVariables(&testVariables);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "functions.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "HELLO world");
}

// Test file with nested template functions
TEST_F(FileBuilderTest, BuildFileWithNestedTemplateFunctions) {
    auto fileData = std::make_unique<FileData>("output/nested.txt", "{{upper(lower(var2))}}");
    fileData->setVariables(&testVariables);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "nested.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "WORLD");
}

// Test file with empty content
TEST_F(FileBuilderTest, BuildFileWithEmptyContent) {
    auto fileData = std::make_unique<FileData>("output/empty.txt", "");
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "empty.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "");
}

// Test file with multi-line content
TEST_F(FileBuilderTest, BuildFileWithMultiLineContent) {
    std::string multiLineContent = "Line 1\nLine 2\nLine 3";
    auto fileData = std::make_unique<FileData>("output/multiline.txt", multiLineContent);
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "multiline.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, multiLineContent);
}

// Test file without variables
TEST_F(FileBuilderTest, BuildFileWithoutVariables) {
    auto fileData = std::make_unique<FileData>("output/novars.txt", "{{var1}}");
    // Don't set variables - should remain as placeholder
    
    builder->build(fileData.get());
    
    std::filesystem::path filePath = testDir / "output" / "novars.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // Should remain as {{var1}} if no variables provided
    EXPECT_EQ(content, "{{var1}}");
}

// Test overwriting existing file
TEST_F(FileBuilderTest, OverwriteExistingFile) {
    // Create file first time
    auto fileData1 = std::make_unique<FileData>("output/overwrite.txt", "Original content");
    builder->build(fileData1.get());
    
    std::filesystem::path filePath = testDir / "output" / "overwrite.txt";
    EXPECT_TRUE(std::filesystem::exists(filePath));
    
    // Overwrite with new content
    auto fileData2 = std::make_unique<FileData>("output/overwrite.txt", "New content");
    builder->build(fileData2.get());
    
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "New content");
}
