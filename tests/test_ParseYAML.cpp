#include <gtest/gtest.h>
#include "../src/services/ParseYAML.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

using namespace TemplateBuilder;

class ParseYAMLTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir = std::filesystem::temp_directory_path() / "template_builder_parse_test";
        std::filesystem::create_directories(testDir);
        originalDir = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
    }
    
    void TearDown() override {
        // Restore original directory
        std::filesystem::current_path(originalDir);
        
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::string createTestYAML(const std::string& content) {
        std::filesystem::path yamlFile = testDir / "test.yaml";
        std::ofstream file(yamlFile);
        file << content;
        file.close();
        return yamlFile.string();
    }
    
    std::filesystem::path testDir;
    std::filesystem::path originalDir;
};

// Test: Load YAML with version
TEST_F(ParseYAMLTest, LoadYAMLWithVersion) {
    std::string yamlContent = R"(
version: 1.0
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    EXPECT_NO_THROW({
        ParserYAML parser(yamlFile);
        EXPECT_EQ(parser.getVersion(), "1.0");
    });
}

// Test: Load YAML without version (should throw)
TEST_F(ParseYAMLTest, LoadYAMLWithoutVersion) {
    std::string yamlContent = R"(
variables: []
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    EXPECT_THROW({
        ParserYAML parser(yamlFile);
    }, std::runtime_error);
}

// Test: Load YAML with unsupported version (should throw)
TEST_F(ParseYAMLTest, LoadYAMLWithUnsupportedVersion) {
    std::string yamlContent = R"(
version: 2.0
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    EXPECT_THROW({
        ParserYAML parser(yamlFile);
    }, UnsupportedTemplateVersion);
}

// Test: Load variables
TEST_F(ParseYAMLTest, LoadVariables) {
    std::string yamlContent = R"(
version: 1.0
variables:
  - name: var1
    type: string
  - name: var2
    type: string
    value: default
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& variables = parser.getVariables();
    
    EXPECT_EQ(variables.size(), 2);
    EXPECT_EQ(variables[0]->getName(), "var1");
    EXPECT_EQ(variables[1]->getName(), "var2");
    EXPECT_TRUE(variables[1]->hasValue());
    EXPECT_EQ(variables[1]->getValue(), "default");
}

// Test: Load prompts
TEST_F(ParseYAMLTest, LoadPrompts) {
    std::string yamlContent = R"(
version: 1.0
variables:
  - name: var1
    type: string
prompts:
  - name: prompt1
    inputs:
      - variable: var1
        input: "Enter value: "
        type: InputString
    result: "Result: {{var1}}"
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& prompts = parser.getPrompts();
    
    EXPECT_EQ(prompts.size(), 1);
    EXPECT_EQ(prompts[0]->getName(), "prompt1");
    EXPECT_EQ(prompts[0]->getInputsCount(), 1);
    EXPECT_EQ(prompts[0]->getInputs()[0]->getType(), PromptType::ptInputString);
}

// Test: Load files
TEST_F(ParseYAMLTest, LoadFiles) {
    std::string yamlContent = R"(
version: 1.0
files:
  - path: "output/file1.txt"
    content: "Static content"
  - path: "output/file2.txt"
    content: "Another file"
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& files = parser.getFiles();
    
    EXPECT_EQ(files.size(), 2);
    EXPECT_EQ(files[0]->getPath(), "output/file1.txt");
    EXPECT_EQ(files[0]->getContent(), "Static content");
    EXPECT_EQ(files[1]->getPath(), "output/file2.txt");
}

// Test: Load folders
TEST_F(ParseYAMLTest, LoadFolders) {
    std::string yamlContent = R"(
version: 1.0
folders:
  - path: "output/subdir1"
  - path: "output/subdir2"
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& folders = parser.getFolders();
    
    EXPECT_EQ(folders.size(), 2);
    EXPECT_EQ(folders[0]->getPath(), "output/subdir1");
    EXPECT_TRUE(folders[0]->getContent().empty());
}

// Test: Load remote files
TEST_F(ParseYAMLTest, LoadRemoteFiles) {
    std::string yamlContent = R"(
version: 1.0
remote:
  - uri: "https://example.com/file1.txt"
    path: "output/file1.txt"
  - uri: "https://example.com/file2.txt"
    path: "output/file2.txt"
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& remoteFiles = parser.getRemoteFiles();
    
    EXPECT_EQ(remoteFiles.size(), 2);
    EXPECT_EQ(remoteFiles[0]->getUri(), "https://example.com/file1.txt");
    EXPECT_EQ(remoteFiles[0]->getPath(), "output/file1.txt");
}

// Test: Load file with prompt reference
TEST_F(ParseYAMLTest, LoadFileWithPrompt) {
    std::string yamlContent = R"(
version: 1.0
variables:
  - name: var1
    type: string
prompts:
  - name: prompt1
    inputs:
      - variable: var1
        input: "Enter value: "
        type: InputString
    result: "Result: {{var1}}"
files:
  - path: "output/file.txt"
    prompt: prompt1
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    ParserYAML parser(yamlFile);
    const auto& files = parser.getFiles();
    
    EXPECT_EQ(files.size(), 1);
    EXPECT_TRUE(files[0]->hasPrompt());
    EXPECT_NE(files[0]->getPrompt(), nullptr);
    EXPECT_EQ(files[0]->getPrompt()->getName(), "prompt1");
}

// Test: Invalid YAML file (should throw)
TEST_F(ParseYAMLTest, InvalidYAMLFile) {
    std::string yamlContent = R"(
version: 1.0
variables:
  - name: var1
    type: invalid_type
)";
    std::string yamlFile = createTestYAML(yamlContent);
    
    EXPECT_THROW({
        ParserYAML parser(yamlFile);
    }, std::runtime_error);
}

// Test: File not found (should throw)
TEST_F(ParseYAMLTest, FileNotFound) {
    EXPECT_THROW({
        ParserYAML parser("nonexistent.yaml");
    }, std::runtime_error);
}

// Test: Empty file name (should throw)
TEST_F(ParseYAMLTest, EmptyFileName) {
    EXPECT_THROW({
        ParserYAML parser("");
    }, std::invalid_argument);
}
