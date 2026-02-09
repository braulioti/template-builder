#include <gtest/gtest.h>
#include "../src/config/Config.hpp"
#include <string>
#include <fstream>

using namespace TemplateBuilder;

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_unique<Config>();
    }

    std::unique_ptr<Config> config;
};

TEST_F(ConfigTest, GetConfigPath_ReturnsNonEmptyPath) {
    const auto& path = config->getConfigPath();
    EXPECT_FALSE(path.empty());
    EXPECT_EQ(path.filename().string(), "config.ini");
}

TEST_F(ConfigTest, GetWithNewKey_CreatesAndReturnsDefault) {
    std::string result = config->get("ConfigTestSection", "UniqueKey1", "defaultValue");
    EXPECT_EQ(result, "defaultValue");
}

TEST_F(ConfigTest, GetWithExistingKey_ReturnsStoredValue) {
    std::string first = config->get("ConfigTestSection", "KeyA", "first");
    ASSERT_EQ(first, "first");
    std::string result = config->get("ConfigTestSection", "KeyA", "second");
    EXPECT_EQ(result, "first");
}

TEST_F(ConfigTest, SetAndSave_ThenGetReturnsValue) {
    config->set("ConfigTestSection", "SetKey", "setValue");
    config->save();
    std::string result = config->get("ConfigTestSection", "SetKey", "wrong");
    EXPECT_EQ(result, "setValue");
}

TEST_F(ConfigTest, GetWithNewKey_PersistsToFile) {
    std::string created = config->get("ConfigTestSection", "PersistKey", "persisted");
    EXPECT_EQ(created, "persisted");
    Config config2;
    std::string result = config2.get("ConfigTestSection", "PersistKey", "other");
    EXPECT_EQ(result, "persisted");
}

TEST_F(ConfigTest, Load_ReloadsFromFile) {
    config->set("ConfigTestSection", "ReloadKey", "reloadValue");
    config->save();
    config->load();
    std::string result = config->get("ConfigTestSection", "ReloadKey", "");
    EXPECT_EQ(result, "reloadValue");
}

TEST_F(ConfigTest, GetWithEmptySection_StoresAndReturnsDefault) {
    std::string result = config->get("", "GlobalKey", "globalDefault");
    EXPECT_EQ(result, "globalDefault");
    std::string again = config->get("", "GlobalKey", "other");
    EXPECT_EQ(again, "globalDefault");
}
