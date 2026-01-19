#pragma once

#include <string>

void showUsage(const char* programName);
bool validateArguments(int argc);
bool fileExists(const std::string& filePath);
int processYamlFile(const std::string& yamlFilePath);
