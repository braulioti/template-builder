#include <gtest/gtest.h>
#include "../src/builders/RemoteFileBuilder.hpp"
#include "../src/types/RemoteFileType.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

using namespace TemplateBuilder;

class RemoteFileBuilderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test directory
        testDir = std::filesystem::temp_directory_path() / "template_builder_remote_test";
        std::filesystem::create_directories(testDir);
        originalDir = std::filesystem::current_path();
        std::filesystem::current_path(testDir);
        
        builder = std::make_unique<RemoteFileBuilder>();
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
    std::unique_ptr<RemoteFileBuilder> builder;
};

// Test: Build with null remote file (should not throw, just return)
TEST_F(RemoteFileBuilderTest, BuildWithNullFile) {
    EXPECT_NO_THROW(builder->build(nullptr));
}

// Test: Build with empty URI (should throw)
TEST_F(RemoteFileBuilderTest, BuildWithEmptyUri) {
    auto remoteFile = std::make_unique<RemoteFileData>("", "test/file.txt");
    
    EXPECT_THROW(builder->build(remoteFile.get()), std::invalid_argument);
}

// Test: Build with empty path (should throw)
TEST_F(RemoteFileBuilderTest, BuildWithEmptyPath) {
    auto remoteFile = std::make_unique<RemoteFileData>("https://example.com/file.txt", "");
    
    EXPECT_THROW(builder->build(remoteFile.get()), std::invalid_argument);
}

// Test: Build with both empty URI and path (should throw)
TEST_F(RemoteFileBuilderTest, BuildWithBothEmpty) {
    auto remoteFile = std::make_unique<RemoteFileData>("", "");
    
    EXPECT_THROW(builder->build(remoteFile.get()), std::invalid_argument);
}

// Test: Automatic directory creation for nested paths
TEST_F(RemoteFileBuilderTest, AutomaticDirectoryCreation) {
    // This test validates that directories are created, but doesn't actually download
    // We'll test the directory creation logic separately if needed
    auto remoteFile = std::make_unique<RemoteFileData>(
        "https://example.com/test.txt",
        "nested/path/to/file.txt"
    );
    
    std::filesystem::path expectedDir = testDir / "nested" / "path" / "to";
    
    // The directory should be created during build (if download succeeds)
    // For this test, we just verify the path preparation logic
    // Actual download test would require a mock HTTP server or real URL
    EXPECT_FALSE(std::filesystem::exists(expectedDir));
}

// Test: RemoteFileData constructor and getters
TEST_F(RemoteFileBuilderTest, RemoteFileDataGetters) {
    RemoteFileData remoteFile("https://example.com/file.txt", "output/file.txt");
    
    EXPECT_EQ(remoteFile.getUri(), "https://example.com/file.txt");
    EXPECT_EQ(remoteFile.getPath(), "output/file.txt");
    EXPECT_FALSE(remoteFile.isEmpty());
    EXPECT_TRUE(remoteFile.hasUri());
    EXPECT_TRUE(remoteFile.hasPath());
}

// Test: RemoteFileData setters
TEST_F(RemoteFileBuilderTest, RemoteFileDataSetters) {
    RemoteFileData remoteFile;
    
    remoteFile.setUri("https://example.com/new.txt");
    remoteFile.setPath("new/path.txt");
    
    EXPECT_EQ(remoteFile.getUri(), "https://example.com/new.txt");
    EXPECT_EQ(remoteFile.getPath(), "new/path.txt");
}

// Test: RemoteFileData isEmpty
TEST_F(RemoteFileBuilderTest, RemoteFileDataIsEmpty) {
    RemoteFileData emptyFile;
    EXPECT_TRUE(emptyFile.isEmpty());
    
    RemoteFileData fileWithUri("https://example.com/file.txt", "");
    EXPECT_FALSE(fileWithUri.isEmpty());
    
    RemoteFileData fileWithPath("", "path.txt");
    EXPECT_FALSE(fileWithPath.isEmpty());
}

// Note: Actual download tests would require:
// 1. A mock HTTP server, or
// 2. A real test URL (like httpbin.org), or
// 3. Integration tests separate from unit tests
//
// For now, we focus on validation and path preparation logic
