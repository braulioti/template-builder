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
- [ ] Convert TFolderBuilder class
- [ ] Implement directory creation (C++17 filesystem)
- [ ] Implement error handling
- [ ] Add unit tests
- [ ] Validate nested directory creation

### unFileBuilder.pas → FileBuilder.hpp/cpp
- [ ] Convert TFileBuilder class
- [ ] Implement file creation
- [ ] Implement UTF-8 encoding
- [ ] Integrate with PromptBuilder
- [ ] Add unit tests
- [ ] Validate automatic parent directory creation

### unPromptBuilder.pas → PromptBuilder.hpp/cpp
- [x] Convert TPromptBuilder class
- [ ] Implement GetInputString (console reading)
- [ ] Implement GetChecklist (interactive interface)
- [ ] Implement GetArrayList (list selection)
- [ ] Implement template variable parsing ({{variable}})
- [ ] Implement template functions (upper, lower, replace)
- [ ] Implement nested function support
- [ ] Implement ResolveVariableValue
- [ ] Implement ParseFunctionExpression
- [ ] Implement ExecuteFunction
- [ ] Add unit tests for each function
- [ ] Validate all prompt types

## Service Conversion

### unParseYAML.pas → ParseYAML.hpp/cpp
- [ ] Convert TParserYAML class
- [ ] Integrate C++ YAML library (yaml-cpp)
- [ ] Implement LoadVariables
- [ ] Implement LoadPrompts
- [ ] Implement LoadFiles
- [ ] Implement LoadFolders
- [ ] Implement ValidateVersion
- [ ] Implement BuildAll
- [ ] Implement error handling
- [ ] Add unit tests
- [ ] Validate parsing of sample YAML files

## Main Program Conversion

### TemplateBuilder.dpr → main.cpp
- [ ] Convert main program
- [ ] Implement command-line argument parsing
- [ ] Implement banner/version display
- [ ] Implement exception handling
- [ ] Implement user-friendly error messages
- [ ] Validate complete end-to-end flow

## Testing and Validation

- [ ] Run all test cases (see use-cases.md)
- [ ] Validate with existing YAML templates (samples/)
- [ ] Test on Windows
- [ ] Test on Linux
- [ ] Test on macOS
- [ ] Validate UTF-8 encoding
- [ ] Validate file and directory creation
- [ ] Validate all prompt types
- [ ] Validate template functions
- [ ] Validate nested functions
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
