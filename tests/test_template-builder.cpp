#include <gtest/gtest.h>
#include "../src/template-builder.hpp"
#include <filesystem>
#include <fstream>

using namespace std;

class TemplateBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test YAML file
        testYamlFile = "test_template.yaml";
        std::ofstream file(testYamlFile);
        file << "version: \"0.2\"\n";
        file << "name: \"Test Template\"\n";
        file.close();
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testYamlFile)) {
            std::filesystem::remove(testYamlFile);
        }
    }

    std::string testYamlFile;
};

TEST_F(TemplateBuilderTest, ValidateArguments_WithValidArgs_ReturnsTrue) {
    int argc = 2;
    EXPECT_TRUE(validateArguments(argc));
}

TEST_F(TemplateBuilderTest, ValidateArguments_WithInsufficientArgs_ReturnsFalse) {
    int argc = 1;
    EXPECT_FALSE(validateArguments(argc));
}

TEST_F(TemplateBuilderTest, FileExists_WithExistingFile_ReturnsTrue) {
    ASSERT_TRUE(std::filesystem::exists(testYamlFile));
    EXPECT_TRUE(fileExists(testYamlFile));
}

TEST_F(TemplateBuilderTest, FileExists_WithNonExistingFile_ReturnsFalse) {
    EXPECT_FALSE(fileExists("nonexistent_file.yaml"));
}

TEST_F(TemplateBuilderTest, ProcessYamlFile_WithValidYaml_ReturnsSuccess) {
    int result = processYamlFile(testYamlFile);
    EXPECT_EQ(0, result);
}

TEST_F(TemplateBuilderTest, ProcessYamlFile_WithInvalidPath_ReturnsError) {
    int result = processYamlFile("nonexistent_file.yaml");
    EXPECT_NE(0, result);
}

TEST_F(TemplateBuilderTest, ProcessYamlFile_WithInvalidYaml_ReturnsError) {
    // Create invalid YAML file
    std::string invalidYaml = "invalid_template.yaml";
    std::ofstream file(invalidYaml);
    file << "invalid: [ yaml content\n"; // Missing closing bracket
    file.close();

    int result = processYamlFile(invalidYaml);
    EXPECT_NE(0, result);

    // Cleanup
    if (std::filesystem::exists(invalidYaml)) {
        std::filesystem::remove(invalidYaml);
    }
}
