#include <gtest/gtest.h>
#include "../src/builders/FolderBuilder.hpp"
#include "../src/types/FileType.hpp"
#include <filesystem>
#include <memory>

using namespace TemplateBuilder;

class FolderBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir = std::filesystem::temp_directory_path() / "template_builder_folder_test";
        std::filesystem::create_directories(testDir);
        originalDir = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
        
        builder = std::make_unique<FolderBuilder>();
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
    std::unique_ptr<FolderBuilder> builder;
};

// Test folder creation with simple path
TEST_F(FolderBuilderTest, BuildSimpleFolder) {
    auto folderData = std::make_unique<FileData>("output", "");
    
    builder->build(folderData.get());
    
    std::filesystem::path folderPath = testDir / "output";
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    EXPECT_TRUE(std::filesystem::is_directory(folderPath));
}

// Test folder creation with nested path
TEST_F(FolderBuilderTest, BuildNestedFolder) {
    auto folderData = std::make_unique<FileData>("level1/level2/level3", "");
    
    std::filesystem::path nestedPath = testDir / "level1" / "level2" / "level3";
    
    // Directory should not exist before build
    EXPECT_FALSE(std::filesystem::exists(nestedPath));
    
    builder->build(folderData.get());
    
    // Directory should be created automatically
    EXPECT_TRUE(std::filesystem::exists(nestedPath));
    EXPECT_TRUE(std::filesystem::is_directory(nestedPath));
}

// Test folder creation with path ending in separator
TEST_F(FolderBuilderTest, BuildFolderWithTrailingSeparator) {
    auto folderData = std::make_unique<FileData>("folder_with_sep/", "");
    
    builder->build(folderData.get());
    
    std::filesystem::path folderPath = testDir / "folder_with_sep";
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    EXPECT_TRUE(std::filesystem::is_directory(folderPath));
}

// Test folder creation with backslash separator (Windows style)
TEST_F(FolderBuilderTest, BuildFolderWithBackslashSeparator) {
    auto folderData = std::make_unique<FileData>("folder_with_backslash\\", "");
    
    builder->build(folderData.get());
    
    std::filesystem::path folderPath = testDir / "folder_with_backslash";
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    EXPECT_TRUE(std::filesystem::is_directory(folderPath));
}

// Test validate nested directory creation - multiple levels
TEST_F(FolderBuilderTest, ValidateNestedDirectoryCreationMultipleLevels) {
    auto folderData = std::make_unique<FileData>("a/b/c/d/e/f/g/deep", "");
    
    std::filesystem::path deepPath = testDir / "a" / "b" / "c" / "d" / "e" / "f" / "g" / "deep";
    
    EXPECT_FALSE(std::filesystem::exists(deepPath));
    builder->build(folderData.get());
    EXPECT_TRUE(std::filesystem::exists(deepPath));
    EXPECT_TRUE(std::filesystem::is_directory(deepPath));
    
    // Verify all parent directories were created
    EXPECT_TRUE(std::filesystem::exists(testDir / "a"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "a" / "b"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "a" / "b" / "c"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "a" / "b" / "c" / "d"));
}

// Test folder creation when directory already exists
TEST_F(FolderBuilderTest, BuildFolderWhenAlreadyExists) {
    auto folderData = std::make_unique<FileData>("existing", "");
    
    // Create directory first
    std::filesystem::path folderPath = testDir / "existing";
    std::filesystem::create_directories(folderPath);
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    
    // Build again - should not throw
    EXPECT_NO_THROW(builder->build(folderData.get()));
    
    // Directory should still exist
    EXPECT_TRUE(std::filesystem::exists(folderPath));
}

// Test empty path throws exception
TEST_F(FolderBuilderTest, EmptyPathThrowsException) {
    auto folderData = std::make_unique<FileData>("", "");
    
    EXPECT_THROW(builder->build(folderData.get()), std::runtime_error);
}

// Test null folder (should not crash)
TEST_F(FolderBuilderTest, BuildWithNullFolder) {
    EXPECT_NO_THROW(builder->build(nullptr));
}

// Test folder path that looks like a file (no separator)
TEST_F(FolderBuilderTest, BuildFolderPathLikeFile) {
    // Path without separator - should create parent directory
    auto folderData = std::make_unique<FileData>("parent/child", "");
    
    builder->build(folderData.get());
    
    // Should create parent directory, not the full path as a directory
    std::filesystem::path parentPath = testDir / "parent";
    EXPECT_TRUE(std::filesystem::exists(parentPath));
    EXPECT_TRUE(std::filesystem::is_directory(parentPath));
}

// Test folder with explicit directory separator
TEST_F(FolderBuilderTest, BuildFolderWithExplicitDirectorySeparator) {
    auto folderData = std::make_unique<FileData>("explicit_dir/", "");
    
    builder->build(folderData.get());
    
    std::filesystem::path folderPath = testDir / "explicit_dir";
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    EXPECT_TRUE(std::filesystem::is_directory(folderPath));
}

// Test multiple folder creations
TEST_F(FolderBuilderTest, BuildMultipleFolders) {
    auto folder1 = std::make_unique<FileData>("folder1", "");
    auto folder2 = std::make_unique<FileData>("folder2", "");
    auto folder3 = std::make_unique<FileData>("folder3", "");
    
    builder->build(folder1.get());
    builder->build(folder2.get());
    builder->build(folder3.get());
    
    EXPECT_TRUE(std::filesystem::exists(testDir / "folder1"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "folder2"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "folder3"));
}

// Test folder in root directory
TEST_F(FolderBuilderTest, BuildFolderInRootDirectory) {
    auto folderData = std::make_unique<FileData>("rootfolder", "");
    
    builder->build(folderData.get());
    
    std::filesystem::path folderPath = testDir / "rootfolder";
    EXPECT_TRUE(std::filesystem::exists(folderPath));
    EXPECT_TRUE(std::filesystem::is_directory(folderPath));
}

// Test complex nested structure
TEST_F(FolderBuilderTest, BuildComplexNestedStructure) {
    auto folder1 = std::make_unique<FileData>("project/src/components", "");
    auto folder2 = std::make_unique<FileData>("project/src/utils", "");
    auto folder3 = std::make_unique<FileData>("project/tests", "");
    
    builder->build(folder1.get());
    builder->build(folder2.get());
    builder->build(folder3.get());
    
    EXPECT_TRUE(std::filesystem::exists(testDir / "project" / "src" / "components"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "project" / "src" / "utils"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "project" / "tests"));
    
    // Verify shared parent directories
    EXPECT_TRUE(std::filesystem::exists(testDir / "project"));
    EXPECT_TRUE(std::filesystem::exists(testDir / "project" / "src"));
}
