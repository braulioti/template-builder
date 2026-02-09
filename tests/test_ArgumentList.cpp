#include <gtest/gtest.h>
#include "../src/services/arguments/ArgumentList.hpp"
#include <string>
#include <vector>

using namespace TemplateBuilder;

class ArgumentListTest : public ::testing::Test {
protected:
};

TEST_F(ArgumentListTest, DefaultConstructed_IsEmpty) {
    ArgumentList list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST_F(ArgumentListTest, FromArgcArgv_WithProgramOnly_HasSizeZero) {
    int argc = 1;
    char arg0[] = "program";
    char* argv[] = {arg0};
    ArgumentList list(argc, argv);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST_F(ArgumentListTest, FromArgcArgv_CopiesArguments) {
    int argc = 4;
    char arg0[] = "program";
    char arg1[] = "first";
    char arg2[] = "second";
    char arg3[] = "third";
    char* argv[] = {arg0, arg1, arg2, arg3};
    ArgumentList list(argc, argv);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.at(0), "first");
    EXPECT_EQ(list.at(1), "second");
    EXPECT_EQ(list.at(2), "third");
}

TEST_F(ArgumentListTest, At_ReturnsCorrectElement) {
    int argc = 3;
    char arg0[] = "prog";
    char arg1[] = "one";
    char arg2[] = "two";
    char* argv[] = {arg0, arg1, arg2};
    ArgumentList list(argc, argv);
    EXPECT_EQ(list.at(0), "one");
    EXPECT_EQ(list.at(0), list[0]);
    EXPECT_EQ(list.at(1), "two");
    EXPECT_EQ(list.at(1), list[1]);
}

TEST_F(ArgumentListTest, At_OutOfRange_Throws) {
    int argc = 2;
    char arg0[] = "prog";
    char arg1[] = "only";
    char* argv[] = {arg0, arg1};
    ArgumentList list(argc, argv);
    EXPECT_THROW(list.at(1), std::out_of_range);
    EXPECT_THROW(list.at(10), std::out_of_range);
}

TEST_F(ArgumentListTest, BeginEnd_AllowIteration) {
    int argc = 3;
    char arg0[] = "p";
    char arg1[] = "a";
    char arg2[] = "b";
    char* argv[] = {arg0, arg1, arg2};
    ArgumentList list(argc, argv);
    std::vector<std::string> copied(list.begin(), list.end());
    EXPECT_EQ(copied.size(), 2u);
    EXPECT_EQ(copied[0], "a");
    EXPECT_EQ(copied[1], "b");
}

TEST_F(ArgumentListTest, FetchAndPrintList_WithEmptyUrl_ReturnsFalse) {
    bool result = ArgumentList::fetchAndPrintList("");
    EXPECT_FALSE(result);
}
