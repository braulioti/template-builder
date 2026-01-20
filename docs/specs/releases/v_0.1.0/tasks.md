# Tasks - Release v0.1.0

## Setup and Infrastructure

- [x] Set up directory structure for C++ code
- [x] Create main CMakeLists.txt file
- [x] Configure YAML library (yaml-cpp or alternative)
- [x] Resolve yaml-cpp dependency issue (tag format and version validation)
- [x] Configure build system for multiple platforms
- [x] Create unit test structure
- [x] Configure CI/CD for multiple platforms using GitHub Actions

## Type Conversion

### unVariableType.pas → VariableType.hpp/cpp
- [x] Convert TVariableType enum to C++ enum class
- [x] Convert TVariable class to C++ class
- [x] Implement getters/setters
- [x] Implement memory management (smart pointers)
- [x] Add unit tests

### unPromptType.pas → PromptType.hpp/cpp
- [x] Convert TPromptType enum to enum class
- [x] Convert TPromptInputOption class
- [x] Convert TPromptInput class
- [x] Convert TPrompt class
- [x] Implement STL containers (std::vector, std::string)
- [x] Add unit tests

### unFileType.pas → FileType.hpp/cpp
- [x] Convert TFileData class
- [x] Implement appropriate references and pointers
- [x] Add unit tests

## Builder Conversion

### unFolderBuilder.pas → FolderBuilder.hpp/cpp
- [x] Convert TFolderBuilder class
- [x] Implement directory creation (C++17 filesystem)
- [x] Implement error handling
- [x] Add unit tests
- [x] Validate nested directory creation

### unFileBuilder.pas → FileBuilder.hpp/cpp
- [x] Convert TFileBuilder class
- [x] Implement file creation
- [x] Implement UTF-8 encoding
- [x] Integrate with PromptBuilder
- [x] Add unit tests
- [x] Validate automatic parent directory creation

### unRemoteFileBuilder.pas → RemoteFileBuilder.hpp/cpp
- [x] Convert TRemoteFileBuilder class
- [x] Convert TRemoteFile type to RemoteFileData
- [x] Integrate HTTP client library (libcurl)
- [x] Implement file download functionality
- [x] Implement automatic directory creation
- [x] Implement error handling for HTTP errors
- [x] Add unit tests
- [x] Validate download with various HTTP status codes

### unPromptBuilder.pas → PromptBuilder.hpp/cpp
- [x] Convert TPromptBuilder class
- [x] Implement GetInputString (console reading)
- [x] Implement GetChecklist (interactive interface)
- [x] Implement GetArrayList (list selection)
- [x] Implement template variable parsing ({{variable}})
- [x] Implement template functions (upper, lower, replace)
- [x] Implement nested function support
- [x] Implement ResolveVariableValue
- [x] Implement ParseFunctionExpression
- [x] Implement ExecuteFunction
- [x] Add unit tests for each function
- [x] Validate all prompt types

## Service Conversion

### unParseYAML.pas → ParseYAML.hpp/cpp
- [x] Convert TParserYAML class
- [x] Integrate C++ YAML library (yaml-cpp)
- [x] Implement LoadVariables
- [x] Implement LoadPrompts
- [x] Implement LoadFiles
- [x] Implement LoadFolders
- [x] Implement LoadRemoteFiles
- [x] Implement ValidateVersion
- [x] Implement BuildAll
- [x] Implement error handling
- [x] Add unit tests
- [x] Validate parsing of sample YAML files
- [x] Test all builders and functions

## Main Program Conversion

### TemplateBuilder.dpr → main.cpp
- [x] Convert main program
- [x] Implement command-line argument parsing
- [x] Implement banner/version display
- [x] Implement exception handling
- [x] Implement user-friendly error messages
- [x] Validate complete end-to-end flow

## Testing and Validation

- [ ] Run all test cases (see use-cases.md)
- [ ] Validate with existing YAML templates (samples/)
- [ ] Test on Windows
- [x] Validate UTF-8 encoding
- [x] Validate file and directory creation
- [x] Validate all prompt types
- [x] Validate template functions
- [x] Validate nested functions
- [ ] Test error handling

## Documentation

- [x] Update README.md with C++ build instructions
- [x] Document dependencies
- [x] Document build process for each platform
- [x] Create installation guide
- [x] Document public APIs (if applicable)
- [x] Update usage documentation

## Build and Distribution

- [x] Configure release build for Windows
- [x] Configure release build for Linux
- [x] Configure release build for macOS
- [x] Fix MSI installer generation for Windows (WiX configuration and workflow)
- [x] Create packaging scripts (optional)
- [x] Test generated binaries
- [x] Prepare release notes

## Cleanup and Finalization

- [ ] Review converted code
- [ ] Apply consistent formatting (clang-format)
- [ ] Remove commented/debug code
- [ ] Validate no memory leaks
- [ ] Retest the generated MSI file (install, uninstall, and functionality)
- [ ] Review and update tasks.md marking completed
- [ ] Prepare PR/Merge for main branch
