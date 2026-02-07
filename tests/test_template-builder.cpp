#include <gtest/gtest.h>
#include "../src/template-builder.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>

using namespace std;

static std::string captureStdout(std::function<void()> action) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    action();
    std::cout.rdbuf(old);
    return buffer.str();
}

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
    char* argv[] = {const_cast<char*>("program"), const_cast<char*>("file.yaml")};
    std::string yamlPath;
    bool forceInteractive = false;
    EXPECT_TRUE(validateArguments(argc, argv, yamlPath, forceInteractive));
    EXPECT_EQ("file.yaml", yamlPath);
}

TEST_F(TemplateBuilderTest, ValidateArguments_WithInteractiveFlag_ReturnsTrue) {
    int argc = 3;
    char* argv[] = {const_cast<char*>("program"), const_cast<char*>("-i"), const_cast<char*>("file.yaml")};
    std::string yamlPath;
    bool forceInteractive = false;
    EXPECT_TRUE(validateArguments(argc, argv, yamlPath, forceInteractive));
    EXPECT_EQ("file.yaml", yamlPath);
    EXPECT_TRUE(forceInteractive);
}

TEST_F(TemplateBuilderTest, ValidateArguments_WithInsufficientArgs_ReturnsFalse) {
    int argc = 1;
    char* argv[] = {const_cast<char*>("program")};
    std::string yamlPath;
    bool forceInteractive = false;
    EXPECT_FALSE(validateArguments(argc, argv, yamlPath, forceInteractive));
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

TEST_F(TemplateBuilderTest, ProcessYamlFile_WithVersion02AndTemplateDetails_DisplaysDetails) {
    // Template with version 0.2 and template details at root
    std::string detailsYaml = "details_template.yaml";
    std::ofstream file(detailsYaml);
    file << "version: \"0.2\"\n";
    file << "name: \"My WordPress Theme\"\n";
    file << "author: \"Jane Doe\"\n";
    file << "email: \"jane@example.com\"\n";
    file << "url: \"https://example.com\"\n";
    file << "language: \"en\"\n";
    file << "folders:\n  - path: \"output\"\n";
    file << "files:\n  - path: \"output/readme.txt\"\n    content: \"Hello\"\n";
    file.close();

    std::string output = captureStdout([&detailsYaml]() {
        processYamlFile(detailsYaml);
    });

    EXPECT_TRUE(output.find("Project: My WordPress Theme") != std::string::npos);
    EXPECT_TRUE(output.find("Author: Jane Doe") != std::string::npos);
    EXPECT_TRUE(output.find("Email: jane@example.com") != std::string::npos);
    EXPECT_TRUE(output.find("URL: https://example.com") != std::string::npos);
    EXPECT_TRUE(output.find("Language: en") != std::string::npos);
    EXPECT_TRUE(output.find("Template successfully generated") != std::string::npos);

    if (std::filesystem::exists(detailsYaml)) {
        std::filesystem::remove(detailsYaml);
    }
    if (std::filesystem::exists("output")) {
        std::filesystem::remove_all("output");
    }
}

TEST_F(TemplateBuilderTest, ProcessYamlFile_WithVersion01_DoesNotDisplayTemplateDetails) {
    std::string v01Yaml = "v01_template.yaml";
    std::ofstream file(v01Yaml);
    file << "version: \"0.1\"\n";
    file << "name: \"Should Not Appear\"\n";
    file << "folders:\n  - path: \"out\"\n";
    file.close();

    std::string output = captureStdout([&v01Yaml]() {
        processYamlFile(v01Yaml);
    });

    EXPECT_TRUE(output.find("Project: Should Not Appear") == std::string::npos);
    EXPECT_TRUE(output.find("Template successfully generated") != std::string::npos);

    if (std::filesystem::exists(v01Yaml)) {
        std::filesystem::remove(v01Yaml);
    }
    if (std::filesystem::exists("out")) {
        std::filesystem::remove_all("out");
    }
}
