#include <gtest/gtest.h>
#include "../src/config/ConfigConstants.hpp"
#include "../src/services/ArgumentParserService.hpp"
#include <functional>
#include <sstream>

using namespace TemplateBuilder;

class ArgumentParserServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<ArgumentParserService>(ConfigConstants::DEFAULT_LIST_URL);
    }

    std::string captureStdout(std::function<void()> action) {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        action();
        std::cout.rdbuf(old);
        return buffer.str();
    }

    std::unique_ptr<ArgumentParserService> service;
};

TEST_F(ArgumentParserServiceTest, Parse_WithYamlPath_ReturnsValid) {
    int argc = 2;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "template.yaml";
    char* argv[] = {arg0, arg1};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.yamlFilePath, "template.yaml");
    EXPECT_FALSE(result.forceInteractive);
    EXPECT_FALSE(result.showHelp);
}

TEST_F(ArgumentParserServiceTest, Parse_WithInteractiveFlag_ReturnsForceInteractive) {
    int argc = 3;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "-i";
    char arg2[] = "template.yaml";
    char* argv[] = {arg0, arg1, arg2};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.yamlFilePath, "template.yaml");
    EXPECT_TRUE(result.forceInteractive);
    EXPECT_FALSE(result.showHelp);
}

TEST_F(ArgumentParserServiceTest, Parse_WithLongInteractiveFlag_ReturnsForceInteractive) {
    int argc = 3;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "--interactive";
    char arg2[] = "template.yaml";
    char* argv[] = {arg0, arg1, arg2};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.yamlFilePath, "template.yaml");
    EXPECT_TRUE(result.forceInteractive);
}

TEST_F(ArgumentParserServiceTest, Parse_WithHelpShort_ReturnsShowHelp) {
    int argc = 2;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "-h";
    char* argv[] = {arg0, arg1};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_TRUE(result.showHelp);
    EXPECT_TRUE(result.yamlFilePath.empty());
}

TEST_F(ArgumentParserServiceTest, Parse_WithHelpLong_ReturnsShowHelp) {
    int argc = 2;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "--help";
    char* argv[] = {arg0, arg1};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_TRUE(result.showHelp);
}

TEST_F(ArgumentParserServiceTest, Parse_WithListShort_ReturnsShowList) {
    int argc = 2;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "-l";
    char* argv[] = {arg0, arg1};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_TRUE(result.showList);
    EXPECT_FALSE(result.showHelp);
    EXPECT_TRUE(result.yamlFilePath.empty());
}

TEST_F(ArgumentParserServiceTest, Parse_WithListLong_ReturnsShowList) {
    int argc = 2;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "--list";
    char* argv[] = {arg0, arg1};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_TRUE(result.showList);
    EXPECT_FALSE(result.showHelp);
}

TEST_F(ArgumentParserServiceTest, Parse_WithInsufficientArgs_ReturnsInvalid) {
    int argc = 1;
    char arg0[] = "TemplateBuilder";
    char* argv[] = {arg0};

    auto result = service->parse(argc, argv);

    EXPECT_FALSE(result.valid);
    EXPECT_TRUE(result.yamlFilePath.empty());
}

TEST_F(ArgumentParserServiceTest, Parse_YamlPathBeforeFlags_ExtractsPath) {
    int argc = 3;
    char arg0[] = "TemplateBuilder";
    char arg1[] = "template.yaml";
    char arg2[] = "-i";
    char* argv[] = {arg0, arg1, arg2};

    auto result = service->parse(argc, argv);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.yamlFilePath, "template.yaml");
}

TEST_F(ArgumentParserServiceTest, ShowUsage_PrintsHelp) {
    std::string output = captureStdout([this]() {
        service->showUsage("TemplateBuilder");
    });

    EXPECT_TRUE(output.find("Usage:") != std::string::npos);
    EXPECT_TRUE(output.find("TemplateBuilder") != std::string::npos);
    EXPECT_TRUE(output.find("file.yaml") != std::string::npos);
    EXPECT_TRUE(output.find("-i") != std::string::npos);
    EXPECT_TRUE(output.find("--interactive") != std::string::npos);
    EXPECT_TRUE(output.find("-l") != std::string::npos);
    EXPECT_TRUE(output.find("--list") != std::string::npos);
    EXPECT_TRUE(output.find("--help") != std::string::npos);
    EXPECT_TRUE(output.find(ConfigConstants::DEFAULT_LIST_URL) != std::string::npos);
}
