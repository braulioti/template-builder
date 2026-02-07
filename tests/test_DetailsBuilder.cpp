#include <gtest/gtest.h>
#include "../src/builders/DetailsBuilder.hpp"
#include <functional>
#include <sstream>

using namespace TemplateBuilder;

class DetailsBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        builder = std::make_unique<DetailsBuilder>();
    }

    std::string captureStdout(std::function<void()> action) {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        action();
        std::cout.rdbuf(old);
        return buffer.str();
    }

    std::unique_ptr<DetailsBuilder> builder;
};

TEST_F(DetailsBuilderTest, DisplayName_WithNonEmptyString_PrintsOutput) {
    std::string output = captureStdout([this]() { builder->displayName("My Project"); });
    EXPECT_EQ(output, "Project: My Project\n");
}

TEST_F(DetailsBuilderTest, DisplayName_WithEmptyString_PrintsNothing) {
    std::string output = captureStdout([this]() { builder->displayName(""); });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayAuthor_WithNonEmptyString_PrintsOutput) {
    std::string output = captureStdout([this]() { builder->displayAuthor("Jane Doe"); });
    EXPECT_EQ(output, "Author: Jane Doe\n");
}

TEST_F(DetailsBuilderTest, DisplayAuthor_WithEmptyString_PrintsNothing) {
    std::string output = captureStdout([this]() { builder->displayAuthor(""); });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayEmail_WithNonEmptyString_PrintsOutput) {
    std::string output = captureStdout([this]() { builder->displayEmail("jane@example.com"); });
    EXPECT_EQ(output, "Email: jane@example.com\n");
}

TEST_F(DetailsBuilderTest, DisplayEmail_WithEmptyString_PrintsNothing) {
    std::string output = captureStdout([this]() { builder->displayEmail(""); });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayUrl_WithNonEmptyString_PrintsOutput) {
    std::string output = captureStdout([this]() { builder->displayUrl("https://example.com"); });
    EXPECT_EQ(output, "URL: https://example.com\n");
}

TEST_F(DetailsBuilderTest, DisplayUrl_WithEmptyString_PrintsNothing) {
    std::string output = captureStdout([this]() { builder->displayUrl(""); });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayLanguage_WithNonEmptyString_PrintsOutput) {
    std::string output = captureStdout([this]() { builder->displayLanguage("pt-BR"); });
    EXPECT_EQ(output, "Language: pt-BR\n");
}

TEST_F(DetailsBuilderTest, DisplayLanguage_WithEmptyString_PrintsNothing) {
    std::string output = captureStdout([this]() { builder->displayLanguage(""); });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayAll_WithAllFields_PrintsAll) {
    std::string output = captureStdout([this]() {
        builder->displayAll("My Template", "Jane Doe", "jane@example.com",
                           "https://example.com", "en");
    });
    EXPECT_TRUE(output.find("Project: My Template") != std::string::npos);
    EXPECT_TRUE(output.find("Author: Jane Doe") != std::string::npos);
    EXPECT_TRUE(output.find("Email: jane@example.com") != std::string::npos);
    EXPECT_TRUE(output.find("URL: https://example.com") != std::string::npos);
    EXPECT_TRUE(output.find("Language: en") != std::string::npos);
}

TEST_F(DetailsBuilderTest, DisplayAll_WithAllEmpty_PrintsNothing) {
    std::string output = captureStdout([this]() {
        builder->displayAll("", "", "", "", "");
    });
    EXPECT_TRUE(output.empty());
}

TEST_F(DetailsBuilderTest, DisplayAll_WithPartialFields_PrintsOnlyNonEmpty) {
    std::string output = captureStdout([this]() {
        builder->displayAll("My Template", "", "jane@example.com", "", "pt-BR");
    });
    EXPECT_TRUE(output.find("Project: My Template") != std::string::npos);
    EXPECT_TRUE(output.find("Author:") == std::string::npos);
    EXPECT_TRUE(output.find("Email: jane@example.com") != std::string::npos);
    EXPECT_TRUE(output.find("URL:") == std::string::npos);
    EXPECT_TRUE(output.find("Language: pt-BR") != std::string::npos);
}
