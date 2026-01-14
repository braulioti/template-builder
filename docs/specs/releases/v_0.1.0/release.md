# Release v0.1.0 - C++ Port

## Release Objective

This release aims to convert the entire Template Builder project from Delphi/Object Pascal to C++, enabling the project to be compiled and distributed across multiple platforms (Windows, Linux, macOS).

## Scope

### Complete Conversion

The conversion will cover all project components:

- **Data Types**: Conversion of all Pascal classes and types to C++ classes
- **Builders**: Conversion of build components (FileBuilder, FolderBuilder, PromptBuilder)
- **Services**: Conversion of YAML parser and orchestration logic
- **Main Program**: Conversion of the application entry point
- **Dependencies**: Replacement of VSoft.YAML library with a C++ YAML library (yaml-cpp or similar)
- **Build System**: Implementation of cross-platform build system (CMake)

### Expected Benefits

- **Cross-Platform**: Native support for Windows, Linux, and macOS
- **Portability**: More portable code, independent of IDE
- **Distribution**: Facilitates distribution through different channels (package managers, GitHub releases)
- **Ecosystem**: Integration with modern C++ tools and libraries

## Technologies and Tools

### Technology Stack

- **Language**: C++17 or higher
- **YAML Library**: yaml-cpp (or cross-platform alternative)
- **Build System**: CMake (minimum version 3.10)
- **Compilers**: GCC, Clang, MSVC
- **Dependency Management**: Conan or vcpkg (optional)

### Target Platforms

- Windows (x64) - MSVC and MinGW
- Linux (x64) - GCC and Clang
- macOS (x64 and ARM64) - Clang

## Compatibility

### Maintained Features

All current features must be preserved:

- ✅ YAML file parsing version 1.0
- ✅ Variable management (string type)
- ✅ Interactive prompts (InputString, Checklist, ArrayList)
- ✅ File generation (static and dynamic)
- ✅ Directory creation
- ✅ Template functions (upper, lower, replace)
- ✅ Nested function support
- ✅ UTF-8 encoding

### YAML Format

The YAML format remains unchanged and fully compatible with existing templates.

## Success Criteria

1. ✅ Project compiles without errors on all target platforms
2. ✅ All test cases pass (see use-cases.md)
3. ✅ Behavior identical to Pascal version
4. ✅ Documentation updated
5. ✅ Functional build system (CMake)
6. ✅ Functional binaries tested on each platform

## Timeline

*To be defined based on team availability*

## Migration Notes

- Pascal code will be kept in the repository as reference during migration
- Conversion will follow modern C++ standards (RAII, smart pointers, etc.)
- CLI interface remains the same to ensure compatibility with existing scripts