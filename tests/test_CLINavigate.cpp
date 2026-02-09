#include <gtest/gtest.h>
#include "cli-utils/CLINavigate.hpp"
#include "types/PromptType.hpp"
#include "types/VariableType.hpp"
#include <memory>
#include <vector>

using namespace TemplateBuilder;

class CLINavigateTest : public ::testing::Test {
protected:
    void SetUp() override {
        testVar = std::make_unique<Variable>("checkVar", VariableType::vtString);
        promptInput = std::make_unique<PromptInput>(PromptType::ptChecklist);
        promptInput->setInput("Select options:");
        promptInput->setVariable(testVar.get());
        promptInput->addOption("Option A", "A");
        promptInput->addOption("Option B", "B");
        promptInput->addOption("Option C", "C");
    }

    std::unique_ptr<Variable> testVar;
    std::unique_ptr<PromptInput> promptInput;
};

TEST_F(CLINavigateTest, DefaultConstructible) {
    CLINavigate nav;
    (void)nav;
}

TEST_F(CLINavigateTest, RunChecklistLoopWithValidPrompt) {
    const auto& options = promptInput->getOptions();
    std::vector<bool> selected(options.size(), false);
    size_t currentIndex = 0;
    bool done = false;

#ifdef _WIN32
    // On Windows the loop waits for keyboard input; skip to avoid blocking in CI
    GTEST_SKIP() << "runChecklistLoop is interactive on Windows, skipped in unit test";
#else
    ChecklistLoopParams params(promptInput.get(), selected, currentIndex, done);
    CLINavigate::runChecklistLoop(params);

    EXPECT_TRUE(done);
    EXPECT_EQ(selected.size(), 3u);
    EXPECT_EQ(currentIndex, 0u);
#endif
}

TEST_F(CLINavigateTest, RunChecklistLoopWithSingleOption) {
    auto singleOption = std::make_unique<PromptInput>(PromptType::ptChecklist);
    singleOption->setInput("Pick one:");
    singleOption->setVariable(testVar.get());
    singleOption->addOption("Only", "only");

    std::vector<bool> selected(1, false);
    size_t currentIndex = 0;
    bool done = false;

#ifdef _WIN32
    GTEST_SKIP() << "runChecklistLoop is interactive on Windows, skipped in unit test";
#else
    ChecklistLoopParams params(singleOption.get(), selected, currentIndex, done);
    CLINavigate::runChecklistLoop(params);

    EXPECT_TRUE(done);
    EXPECT_EQ(selected.size(), 1u);
#endif
}

TEST_F(CLINavigateTest, RunChecklistLoopPreservesSelectedSize) {
    const auto& options = promptInput->getOptions();
    std::vector<bool> selected(options.size(), true);  // all selected
    size_t currentIndex = 1;
    bool done = false;

#ifdef _WIN32
    GTEST_SKIP() << "runChecklistLoop is interactive on Windows, skipped in unit test";
#else
    ChecklistLoopParams params(promptInput.get(), selected, currentIndex, done);
    CLINavigate::runChecklistLoop(params);

    EXPECT_TRUE(done);
    EXPECT_EQ(selected.size(), 3u);
    // On non-Windows we exit immediately without toggling; selected stays as-is
    EXPECT_TRUE(selected[0]);
    EXPECT_TRUE(selected[1]);
    EXPECT_TRUE(selected[2]);
#endif
}

TEST_F(CLINavigateTest, SelectFromList_WithEmptyList_ReturnsNullopt) {
    std::vector<std::string> items;
    auto result = CLINavigate::selectFromList(items);
    EXPECT_FALSE(result.has_value());
}
