# Template Builder - Version: 0.1.0

[![X: @_brau_io](https://img.shields.io/badge/contact-@_brau_io-blue.svg?style=flat)](https://x.com/_brau_io)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/f69823e9d31f443d89212528e308c716)](https://app.codacy.com/gh/braulioti/template-builder/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions&logoColor=white)](./.github)
[![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white)](https://visualstudio.microsoft.com/vs/features/cplusplus/)
[![Docker](https://img.shields.io/badge/Docker-2496ED?logo=docker&logoColor=fff)](https://www.docker.com/)
[![Angular 21](https://img.shields.io/badge/Angular-21-DD0031?style=for-the-badge&logo=angular&logoColor=white)](https://angular.dev/)
[![Jest](https://img.shields.io/badge/Jest-323330?style=for-the-badge&logo=Jest&logoColor=white)](https://jestjs.io/)
[![Licence](https://img.shields.io/github/license/Ileriayo/markdown-badges?style=for-the-badge)](./LICENSE)

Template Builder is a CLI tool that allows users to create custom project templates from YAML configuration files. The tool generates ready-made project structures by creating folders, generating files, and collecting user input through interactive CLI prompts defined in YAML files.

Template Builder is created and maintained by [Bráulio Figueiredo](https://brau.io).

## Table of Contents

- [Project Structure](#project-structure)
- [Release Calendar](#release-calendar)
  - [Status legend](#status-legend)
- [Technologies](#technologies)
- [Build and Run](#build-and-run)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
  - [Running the Application](#running-the-application)
- [Deploy](#deploy)
  - [Nexus (Docker image)](#nexus-docker-image)
  - [Production server (docker-compose)](#production-server-docker-compose)
  - [Local docker-compose usage](#local-docker-compose-usage)
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

| Date         | Description                                                   | Version | Status       |
|--------------|---------------------------------------------------------------|---------|--------------|
| Feb 5, 2026  | Created a new version in C++ including multi-platform support | 0.1.0   | Published    |
| Mar 10, 2026 |                                                               | 0.2.0   | Developing   |
| Abr 10, 2026 |                                                               | 0.3.0   | Planned      |
|              |                                                               | 1.0.0   | On Hold      |

### Status legend

| Status                        | Description                                                                |
|-------------------------------|----------------------------------------------------------------------------|
| **Developing**                | Version under active development; features and fixes in progress.          |
| **Available for Publication** | Version is feature-complete and ready to be published or released.         |
| **Published**                 | Version has been released and is available to users.                       |
| **Planned**                   | Version is scheduled for a future release; scope may still be defined.     |
| **On Hold**                   | Work on this version is paused; may resume later.                          |
| **Deprecated**                | Version is no longer recommended; users should upgrade to a newer version. |

## Technologies

- C++ 17
- CMake (minimum version 3.10)
- Google Test (v1.14.0)
- YAML Library: yaml-cpp
- WIX Toolset V6.0
- Angular 21
- Jest + jest-preset-angular
- Nexus

## Build and Run

### Prerequisites

- CMake (version 3.10 or higher)
- C++ compiler with C++17 support:
  - **Windows**: MSVC or MinGW
  - **Linux**: GCC or Clang
  - **macOS**: Clang

### Building the Project

1. **Install vcpkg (optional, for dependency management)**:
   ```bash
   cd C:\
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   bootstrap-vcpkg.bat
   ```
   
   **Note**: vcpkg is optional. The project uses CMake FetchContent to automatically download dependencies (yaml-cpp, libcurl) if they are not found on the system.

2. **Clone the repository**:
   ```bash
   git clone https://github.com/braulioti/template-builder.git
   cd template-builder
   ```

3. **Configure CMake**:
   ```bash
   cmake -B cmake-build-debug -S .
   ```
   
   For Debug build:
   ```bash
   cmake -B cmake-build-debug -S . -DCMAKE_BUILD_TYPE=Debug
   ```
   
   To build with tests:
   ```bash
   cmake -B cmake-build-debug -S . -DBUILD_TESTS=ON
   ```

4. **Build the project**:
   ```bash
   cmake --build cmake-build-debug
   ```
   
   On Windows with MSVC (specify config):
   ```bash
   cmake --build cmake-build-debug --config Release
   ```

5. **Executable location**:
   - **Linux/macOS**: `cmake-build-debug/bin/TemplateBuilder`
   - **Windows (Release)**: `cmake-build-debug/bin/Release/TemplateBuilder.exe`
   - **Windows (Debug)**: `cmake-build-debug/bin/Debug/TemplateBuilder.exe`

### Running the Application

Execute the application with a YAML template file:

**Linux/macOS**:
```bash
./cmake-build-debug/bin/TemplateBuilder samples/wordpress-theme.yaml
```

**Windows**:
```bash
cmake-build-debug\bin\TemplateBuilder.exe samples\wordpress-theme.yaml
```

**Note:** If running from an IDE (e.g. CLion) and prompts do not appear, add the `-i` flag to force interactive mode:
```bash
./cmake-build-debug/bin/TemplateBuilder -i samples/wordpress-theme.yaml
```

The application will:
1. Parse the YAML file
2. Validate the template version
3. Execute interactive prompts (if any)
4. Generate files and folders according to the template
5. Display a success message upon completion

Sample templates are available in the `samples/` directory

## Deploy

### Deploy and Release (CI/CD)

When creating a **release** (tag `v*`), the continuous integration:

1. **Builds the Docker image** and publishes it to **Nexus** (image registry).
2. **Publishes docker-compose** to the production server and runs the application.

### Nexus (Docker image)

Configure in the repository (Settings → Secrets and variables → Actions):

| Type     | Name             | Description                                |
|----------|------------------|--------------------------------------------|
| Variable | `NEXUS_REGISTRY` | Nexus host (e.g. `nexus.company.com:8082`) |
| Secret   | `NEXUS_USERNAME` | Nexus username                             |
| Secret   | `NEXUS_PASSWORD` | Nexus password                             |

If `NEXUS_REGISTRY` is not defined, the workflow still builds the image but does not push to Nexus.

### Production server (docker-compose)

To publish docker-compose and run the app in production:

| Type     | Name                     | Description                                                           |
|----------|--------------------------|-----------------------------------------------------------------------|
| Variable | `PRODUCTION_HOST`        | Server host or IP (e.g. `app.company.com`)                            |
| Variable | `PRODUCTION_USER`        | SSH user (e.g. `deploy`)                                              |
| Variable | `PRODUCTION_PORT`        | (Optional) Host port for the container (default: `80`)                |
| Variable | `PRODUCTION_DEPLOY_PATH` | (Optional) Directory on the server (default: `/opt/template-builder`) |
| Secret   | `PRODUCTION_SSH_KEY`     | SSH private key for server access                                     |

The deploy job creates an `.env` file on the server with the latest image published to Nexus (`IMAGE`) and the port (`PORT` = `PRODUCTION_PORT` or 80), then runs `docker compose up -d`.

On the production server you need:

- Docker and Docker Compose (v2) installed.
- The SSH user with permission to run `docker compose` (e.g. user in the `docker` group).
- If Nexus is private: run `docker login` on the server to the Nexus registry (or configure `~/.docker/config.json`) so that `docker compose pull` can download the image.

### Local docker-compose usage

To run with the Nexus image:

```bash
export IMAGE=nexus.company.com:8082/template-builder:1.0.0
docker compose up -d
```

Or define the port:

```bash
export IMAGE=nexus.company.com:8082/template-builder:1.0.0
export PORT=8080
docker compose up -d
```

## Tests

The project uses Google Test framework for unit testing. This section explains how to create and run tests.

### Running Tests

#### Enabling Tests in CLion

1. **Open CMake Settings**:
   - Go to **File → Settings → Build, Execution, Deployment → CMake**
   - Select your configuration (e.g., "Debug")
   - In **CMake options**, add:
     ```
     -DBUILD_TESTS=ON
     ```
   - Click **Apply** and then **OK**

2. **Reload CMake Project**:
   - Go to **File → Reload CMake Project** (or press `Ctrl+Shift+O`)
   - CLion will reconfigure CMake with tests enabled

3. **Build the project**:
   - The test executables will be built automatically when you build the project
   - You can also build specific tests from the **Build** menu

4. **Run tests**:
   - Right-click on any test file in the project tree and select **Run**
   - Or use **Run → Run 'All Tests'** to run all tests
   - Tests will appear in the **Run** tool window

#### Enabling Tests via Command Line

1. **Configure CMake with tests enabled**:
   ```bash
   cmake -B cmake-build-debug -S . -DBUILD_TESTS=ON
   ```

2. **Build the project**:
   ```bash
   cmake --build cmake-build-debug
   ```

3. **Run all tests using CTest**:
   ```bash
   cd cmake-build-debug
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
