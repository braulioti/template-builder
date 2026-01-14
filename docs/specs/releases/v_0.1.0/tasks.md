# Tasks - Release v0.1.0

## Setup and Infrastructure

- [x] Set up directory structure for C++ code
- [x] Create main CMakeLists.txt file
- [x] Configure YAML library (yaml-cpp or alternative)
- [ ] Resolve yaml-cpp dependency issue (tag format and version validation)
- [x] Configure build system for multiple platforms
- [x] Create unit test structure
- [x] Configure CI/CD for multiple platforms using GitHub Actions

## Type Conversion

### unVariableType.pas → VariableType.hpp/cpp
- [ ] Convert TVariableType enum to C++ enum class
- [ ] Convert TVariable class to C++ class
- [ ] Implement getters/setters
- [ ] Implement memory management (smart pointers)
- [ ] Add unit tests

### unPromptType.pas → PromptType.hpp/cpp
- [ ] Convert TPromptType enum to enum class
- [ ] Convert TPromptInputOption class
- [ ] Convert TPromptInput class
- [ ] Convert TPrompt class
- [ ] Implement STL containers (std::vector, std::string)
- [ ] Add unit tests

### unFileType.pas → FileType.hpp/cpp
- [ ] Convert TFileData class
- [ ] Implement appropriate references and pointers
- [ ] Add unit tests

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
- [ ] Convert TPromptBuilder class
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

- [ ] Update README.md with C++ build instructions
- [ ] Document dependencies
- [ ] Document build process for each platform
- [ ] Create installation guide
- [ ] Document public APIs (if applicable)
- [ ] Update usage documentation

## Build and Distribution

- [ ] Configure release build for Windows
- [ ] Configure release build for Linux
- [ ] Configure release build for macOS
- [ ] Create packaging scripts (optional)
- [ ] Test generated binaries
- [ ] Prepare release notes

## Cleanup and Finalization

- [ ] Review converted code
- [ ] Apply consistent formatting (clang-format)
- [ ] Remove commented/debug code
- [ ] Validate no memory leaks
- [ ] Review and update tasks.md marking completed
- [ ] Prepare PR/Merge for main branch