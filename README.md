# Template Builder - Version: 0.1.0

[![X: @_brau_io](https://img.shields.io/badge/contact-@_brau_io-blue.svg?style=flat)](https://x.com/_brau_io)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/f69823e9d31f443d89212528e308c716)](https://app.codacy.com/gh/braulioti/template-builder/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions&logoColor=white)](./.github)
[![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white)](https://visualstudio.microsoft.com/vs/features/cplusplus/)
[![Licence](https://img.shields.io/github/license/Ileriayo/markdown-badges?style=for-the-badge)](./LICENSE)

Template Builder is a CLI tool that allows users to create custom project templates from YAML configuration files. The tool generates ready-made project structures by creating folders, generating files, and collecting user input through interactive CLI prompts defined in YAML files.

Template Builder is created and maintained by [Bráulio Figueiredo](https://brau.io).

## Table of Contents

- [Project Structure](#project-structure)
- [Release Calendar](#release-calendar)
- [Technologies](#technologies)
- [Build and Run](#build-and-run)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
  - [Running the Application](#running-the-application)
- [Tests](#tests)
  - [Running Tests](#running-tests)
- [Generating Windows MSI Installer](#generating-windows-msi-installer)
- [Troubleshooting](#troubleshooting)
- [Versioning](#versioning)
- [Author](#author)

## Project Structure

```
template-builder/
├── .github/                   # GitHub Actions workflows (CI/CD)
├── src/                       # Source code (C++)
├── tests/                     # Unit tests
├── samples/                   # Example YAML templates
├── docs/                      # Project documentation
├── installer/                 # Installation scripts and configurations
├── libs/                      # Files necessary for generating Windows installer
├── old_src/                   # Legacy Pascal source code (reference)
├── CMakeLists.txt            # CMake build configuration
├── README.md                 # Main documentation
├── CHANGELOG.md              # Change history
├── CONTRIBUTING.md           # Contribution guide
├── TROUBLESHOOTING.md        # Troubleshooting guide
└── LICENSE                   # Project license
```

## Release Calendar

| Date | Description | Version | Status |
|------|-------------|---------|--------|
| March 1, 2026 | In this version we will refactor from Delphi to C++ including multi-platform support | 0.1.0 | Em desenvolvimento |
| March 1, 2026 |  | 0.2.0 |  |
| March 1, 2026 |  | 1.0.0 |  |

## Technologies

- C++ 17
- CMake (minimum version 3.10)
- Google Test (v1.14.0)
- YAML Library: yaml-cpp
- WIX Toolset V6.0

## Build and Run

### Prerequisites

- CMake (version 3.10 or higher)
- C++ compiler with C++17 support:
  - **Windows**: MSVC or MinGW
  - **Linux**: GCC or Clang
  - **macOS**: Clang

### Building the Project

1. **Clone the repository**:
   ```bash
   git clone https://github.com/braulioti/template-builder.git
   cd template-builder
   ```

2. **Configure CMake**:
   ```bash
   cmake -B build -S .
   ```
   
   For Debug build:
   ```bash
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
   ```
   
   To build with tests:
   ```bash
   cmake -B build -S . -DBUILD_TESTS=ON
   ```

3. **Build the project**:
   ```bash
   cmake --build build
   ```
   
   On Windows with MSVC (specify config):
   ```bash
   cmake --build build --config Release
   ```

4. **Executable location**:
   - **Linux/macOS**: `build/bin/TemplateBuilder`
   - **Windows (Release)**: `build/bin/Release/TemplateBuilder.exe`
   - **Windows (Debug)**: `build/bin/Debug/TemplateBuilder.exe`

### Running the Application

Execute the application with a YAML template file:

**Linux/macOS**:
```bash
./build/bin/TemplateBuilder samples/sample.yaml
```

**Windows**:
```bash
build\bin\Release\TemplateBuilder.exe samples\sample.yaml
```

The application will:
1. Parse the YAML file
2. Validate the template version
3. Execute interactive prompts (if any)
4. Generate files and folders according to the template
5. Display a success message upon completion

Sample templates are available in the `samples/` directory

## Tests

The project uses Google Test framework for unit testing. This section explains how to create and run tests.

### Running Tests

1. **Configure CMake with tests enabled**:
   ```bash
   cmake -B build -S . -DBUILD_TESTS=ON
   ```

2. **Build the project**:
   ```bash
   cmake --build build
   ```

3. **Run all tests using CTest**:
   ```bash
   cd build
   ctest --output-on-failure
   ```

   Or run individual test executables:
   ```bash
   ./tests/test_VariableType
   ./tests/test_PromptType
   ./tests/test_FileType
   ```

## Generating Windows MSI Installer

After building the application, you can generate the Windows MSI installer using the provided PowerShell script.

**Prerequisites:**
- WiX Toolset installed on your system
- The application must be built before generating the installer

**Steps:**

1. Ensure the application build is complete (see [Building the Project](#building-the-project))

2. Run the build script from the project root:
   ```powershell
   .\installer\build-msi.ps1
   ```

The script will generate the MSI installer file in the `installer/` directory.
## Troubleshooting

[Click here](TROUBLESHOOTING.md) for solutions to the main compilation and deployment problems you may encounter while working with this project.

## Versioning

Template Builder "Semantic Versioning" guidelines whenever possible.
Updates are numbered as follows:

`<major>.<minor>.<patch>`

Built on the following guidelines:

* Breaking compatibility with the previous version will be updated in "major"
* New implementations and features in "minor"
* Bug fixes in "patch"

For more information about SemVer, please visit http://semver.org.

## Author
- Email: braulio@braulioti.com.br
- X: https://x.com/_brau_io
- GitHub: https://github.com/braulioti
- Website: http://brau.io 
