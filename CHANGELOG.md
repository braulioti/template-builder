# Changelog

## [Version 0.1.0] - 2026-02-18

This release marks the initial C++ port of the Template Builder project in modern C++17. The primary objective is to enable cross-platform compilation and distribution across Windows, Linux, and macOS while maintaining full compatibility with existing YAML templates. This conversion establishes a solid foundation with a CMake-based build system, integrates yaml-cpp for YAML parsing, implements Google Test framework for comprehensive unit testing, and sets up CI/CD pipelines for continuous integration across all target platforms. The conversion preserves all existing functionality including variable management, interactive prompts, file generation, directory creation, template functions, and UTF-8 encoding support, ensuring seamless migration for existing users.

### Feature
- Created cross-platform CMake build system for Windows, Linux, and macOS.
- Created structure to import YAML files (variables, prompts, files, folders, remote files).
- Created release workflow for building artifacts across all platforms (Windows MSI, Linux tar.gz, macOS tar.gz and PKG).
- Created structure for folder generation.
- Created dynamic file generation (content and prompt-based).
- Created structure for remote file download.
- Created prompt builder for interactive prompts (InputString, Checklist, ArrayList).
- Created template function `upper` for converting variable values to uppercase.
- Created template function `lower` for converting variable values to lowercase.
- Created template function `replace` for replacing text in variable values.
- Created basic frontend application.
- Set up GitHub Actions CI/CD pipeline for multiple platforms (Windows MSVC, Linux GCC, macOS x64).
- Set up MSI installer generation for Windows using WiX Toolset.
- Created PowerShell script (build-msi.ps1) for generating MSI installer on Windows.

### Documentation
- Created README.md with general project documentation.
- Created web documentation project in Angular (frontend) with pages for installation, usage, build template (variables, remote files, files, prompts, functions), and terms of use.
- Created LICENSE file with MIT license.
- Created TROUBLESHOOTING.md file with solutions to main compilation and deployment problems.
- Created CONTRIBUTING.md file with instructions on how to contribute to the project.
- Created release documentation in docs/specs/releases/v_0.1.0/ with use-cases, tasks, and release notes.
