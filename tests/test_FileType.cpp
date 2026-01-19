#include <gtest/gtest.h>
#include "../src/types/FileType.hpp"
#include "../src/types/PromptType.hpp"
#include "../src/types/VariableType.hpp"
#include <vector>
#include <memory>

using namespace TemplateBuilder;

class FileTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test variables
        testVariable1 = std::make_unique<Variable>("var1", VariableType::vtString, "value1");
        testVariable2 = std::make_unique<Variable>("var2", VariableType::vtString, "value2");
        
        testVariables.push_back(testVariable1.get());
        testVariables.push_back(testVariable2.get());
        
        // Create test prompt
        testPrompt = std::make_unique<Prompt>("testPrompt");
        testPrompt->setResult("Result template");
    }

    void TearDown() override {
        // Cleanup
    }

    std::unique_ptr<Variable> testVariable1;
    std::unique_ptr<Variable> testVariable2;
    std::vector<Variable*> testVariables;
    std::unique_ptr<Prompt> testPrompt;
};

TEST_F(FileTypeTest, DefaultConstructor) {
    FileData fileData;
    EXPECT_EQ(fileData.getPath(), "");
    EXPECT_EQ(fileData.getContent(), "");
    EXPECT_EQ(fileData.getPrompt(), nullptr);
    EXPECT_EQ(fileData.getVariables(), nullptr);
    EXPECT_FALSE(fileData.hasPrompt());
    EXPECT_FALSE(fileData.hasVariables());
    EXPECT_TRUE(fileData.isEmpty());
}

TEST_F(FileTypeTest, ConstructorWithPathAndContent) {
    FileData fileData("path/to/file.txt", "File content");
    EXPECT_EQ(fileData.getPath(), "path/to/file.txt");
    EXPECT_EQ(fileData.getContent(), "File content");
    EXPECT_EQ(fileData.getPrompt(), nullptr);
    EXPECT_EQ(fileData.getVariables(), nullptr);
    EXPECT_FALSE(fileData.hasPrompt());
    EXPECT_FALSE(fileData.hasVariables());
    EXPECT_FALSE(fileData.isEmpty());
}

TEST_F(FileTypeTest, ConstructorWithEmptyContent) {
    FileData fileData("path/to/folder", "");
    EXPECT_EQ(fileData.getPath(), "path/to/folder");
    EXPECT_EQ(fileData.getContent(), "");
    EXPECT_FALSE(fileData.isEmpty());  // Path is not empty
}

TEST_F(FileTypeTest, Setters) {
    FileData fileData;
    
    fileData.setPath("new/path/file.txt");
    EXPECT_EQ(fileData.getPath(), "new/path/file.txt");
    
    fileData.setContent("New content");
    EXPECT_EQ(fileData.getContent(), "New content");
    
    fileData.setPrompt(testPrompt.get());
    EXPECT_EQ(fileData.getPrompt(), testPrompt.get());
    EXPECT_TRUE(fileData.hasPrompt());
    
    fileData.setVariables(&testVariables);
    EXPECT_EQ(fileData.getVariables(), &testVariables);
    EXPECT_TRUE(fileData.hasVariables());
}

TEST_F(FileTypeTest, SetPrompt) {
    FileData fileData("path/file.txt", "content");
    EXPECT_FALSE(fileData.hasPrompt());
    
    fileData.setPrompt(testPrompt.get());
    EXPECT_TRUE(fileData.hasPrompt());
    EXPECT_EQ(fileData.getPrompt(), testPrompt.get());
    EXPECT_EQ(fileData.getPrompt()->getName(), "testPrompt");
}

TEST_F(FileTypeTest, SetVariables) {
    FileData fileData("path/file.txt", "content");
    EXPECT_FALSE(fileData.hasVariables());
    
    fileData.setVariables(&testVariables);
    EXPECT_TRUE(fileData.hasVariables());
    EXPECT_EQ(fileData.getVariables(), &testVariables);
    EXPECT_EQ(fileData.getVariables()->size(), 2);
    EXPECT_EQ((*fileData.getVariables())[0], testVariable1.get());
    EXPECT_EQ((*fileData.getVariables())[1], testVariable2.get());
}

TEST_F(FileTypeTest, ClearPrompt) {
    FileData fileData("path/file.txt", "content");
    fileData.setPrompt(testPrompt.get());
    EXPECT_TRUE(fileData.hasPrompt());
    
    fileData.setPrompt(nullptr);
    EXPECT_FALSE(fileData.hasPrompt());
    EXPECT_EQ(fileData.getPrompt(), nullptr);
}

TEST_F(FileTypeTest, ClearVariables) {
    FileData fileData("path/file.txt", "content");
    fileData.setVariables(&testVariables);
    EXPECT_TRUE(fileData.hasVariables());
    
    fileData.setVariables(nullptr);
    EXPECT_FALSE(fileData.hasVariables());
    EXPECT_EQ(fileData.getVariables(), nullptr);
}

TEST_F(FileTypeTest, CompleteFileData) {
    FileData fileData("output/file.txt", "Initial content");
    fileData.setPrompt(testPrompt.get());
    fileData.setVariables(&testVariables);
    
    EXPECT_EQ(fileData.getPath(), "output/file.txt");
    EXPECT_EQ(fileData.getContent(), "Initial content");
    EXPECT_TRUE(fileData.hasPrompt());
    EXPECT_TRUE(fileData.hasVariables());
    EXPECT_FALSE(fileData.isEmpty());
    EXPECT_EQ(fileData.getPrompt()->getName(), "testPrompt");
    EXPECT_EQ(fileData.getVariables()->size(), 2);
}

TEST_F(FileTypeTest, FolderData) {
    // Folders typically have path but empty content
    FileData folderData("output/subdirectory", "");
    
    EXPECT_EQ(folderData.getPath(), "output/subdirectory");
    EXPECT_EQ(folderData.getContent(), "");
    EXPECT_FALSE(folderData.isEmpty());  // Path is not empty
}

TEST_F(FileTypeTest, IsEmpty) {
    FileData empty1;
    EXPECT_TRUE(empty1.isEmpty());
    
    FileData empty2("", "");
    EXPECT_TRUE(empty2.isEmpty());
    
    FileData withPath("path", "");
    EXPECT_FALSE(withPath.isEmpty());
    
    FileData withContent("", "content");
    EXPECT_FALSE(withContent.isEmpty());
    
    FileData withBoth("path", "content");
    EXPECT_FALSE(withBoth.isEmpty());
}
