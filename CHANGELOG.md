# Changelog


## [Version 0.1.0] - XXXX-XX-XX

This release marks the initial C++ port of the Template Builder project, converting the entire codebase from Delphi/Object Pascal to modern C++17. The primary objective is to enable cross-platform compilation and distribution across Windows, Linux, and macOS while maintaining full compatibility with existing YAML templates. This conversion establishes a solid foundation with a CMake-based build system, integrates yaml-cpp for YAML parsing, implements Google Test framework for comprehensive unit testing, and sets up CI/CD pipelines for continuous integration across all target platforms. The conversion preserves all existing functionality including variable management, interactive prompts, file generation, directory creation, template functions, and UTF-8 encoding support, ensuring seamless migration for existing users.

### Feature
- Created cross-platform CMake build system for Windows, Linux, and macOS.
- Set up directory structure for C++ code conversion from Delphi/Object Pascal.
- Integrated yaml-cpp library for YAML file parsing using FetchContent.
- Configured Google Test framework for unit testing with FetchContent.
- Set up GitHub Actions CI/CD pipeline for multiple platforms (Windows MSVC, Linux GCC, macOS x64).
- Converted TVariableType enum from Pascal to C++ enum class (VariableType).
- Converted TVariable class from Pascal to C++ class with smart pointers memory management.
- Implemented VariableType getters and setters.
- Converted TPromptType enum from Pascal to C++ enum class (PromptType).
- Converted TPromptInputOption class from Pascal to C++.
- Converted TPromptInput class from Pascal to C++ using STL containers (std::vector, std::string).
- Converted TPrompt class from Pascal to C++.
- Converted TFileData class from Pascal to C++ (FileType).
- Implemented appropriate references and pointers for FileType.
- Created unit tests for VariableType class (test_VariableType).
- Created unit tests for PromptType class (test_PromptType).
- Created unit tests for FileType class (test_FileType).
- Implemented C++17 standard with modern practices (RAII, smart pointers).
- Added support for C++17 filesystem library for cross-platform file operations.
- Configured build output directories and platform-specific compiler options.
- Set up MSI installer generation for Windows using WiX Toolset.
- Created PowerShell script (build-msi.ps1) for generating MSI installer on Windows.
- Created release workflow for building artifacts across all platforms (Windows MSI, Linux tar.gz, macOS tar.gz and PKG).
- Converted TPromptBuilder class from Pascal to C++ (PromptBuilder).
- Converted TFileBuilder class from Pascal to C++ (FileBuilder).

### Deprecated

### Fix

### Documentation
- Created LICENSE file with MIT license.
- Created TROUBLESHOOTING.md file with solutions to main compilation and deployment problems.
- Created CONTRIBUTING.md file with instructions on how to contribute to the project.
- Created release documentation in docs/specs/releases/v_0.1.0/ with use-cases, tasks, and release notes.
- Documented CMake build process and dependencies.
