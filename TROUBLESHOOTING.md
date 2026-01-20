# Troubleshooting

## About this file

This file contains solutions to the main compilation and deployment problems you may encounter while working with this project.

## Build Errors

### CMake Error: CMAKE_CXX_COMPILER not set or No C++ compiler found

**Solution:**

Install a C++ compiler with C++17 support:

1. **Windows**: Install Visual Studio 2022 with "Desktop development with C++" workload, or install Visual C++ Build Tools
2. **Linux**: Install GCC or Clang: `sudo apt-get install build-essential` (Ubuntu/Debian)
3. **macOS**: Install Xcode Command Line Tools: `xcode-select --install`

Verify installation:
```bash
# Windows
where.exe cl

# Linux/macOS
g++ --version
```

---

### yaml-cpp/yaml.h: No such file or directory

**Solution:**

The project uses CMake FetchContent to automatically download yaml-cpp. If you still get this error:

1. Ensure CMake version 3.10 or higher is installed
2. Reconfigure CMake:
   ```bash
   cmake -B build -S .
   ```

If using vcpkg instead, pass the toolchain file:
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

### Could not find a package configuration file provided by "yaml-cpp"

**Solution:**

The project automatically downloads yaml-cpp via FetchContent. If you're using vcpkg and see this error:

1. Install yaml-cpp via vcpkg:
   ```bash
   vcpkg install yaml-cpp
   ```

2. Configure CMake with vcpkg toolchain:
   ```bash
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

---

### Linker errors (unresolved external symbol)

**Solution:**

Ensure yaml-cpp is properly linked in your CMakeLists.txt. The main CMakeLists.txt already includes this:

```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
```

If you're creating a new target, make sure to link yaml-cpp:
```cmake
target_link_libraries(your_target PRIVATE yaml-cpp)
```

---

### Tests fail to build or run

**Solution:**

1. Enable tests when configuring CMake:
   ```bash
   cmake -B build -S . -DBUILD_TESTS=ON
   ```

2. Build the project:
   ```bash
   cmake --build build
   ```

3. Run tests:
   ```bash
   cd build
   ctest --output-on-failure
   ```

Ensure Google Test is available. The project automatically downloads it via FetchContent when `BUILD_TESTS=ON`.

---

### Build succeeds locally but fails in CI/CD

**Solution:**

Check that your CI/CD pipeline:
1. Has all required dependencies installed (CMake, C++ compiler)
2. Configures CMake correctly for the platform
3. Uses the correct build commands

For GitHub Actions, the provided workflows in `.github/workflows/` already handle this correctly.

---

### CMake Error: Generator mismatch or incompatible build directory

**Symptoms:**
- Error: "Cannot generate into [directory]. It was created with incompatible generator"
- Error: "Error: generator : Visual Studio 18 2026. Does not match the generator used previously"
- Build step for curl failed with generator errors

**Solution:**

This error occurs when CMake tries to use a different generator than the one used to create the build directory.

1. **Clean the build directory completely:**
   ```powershell
   # Windows PowerShell
   Remove-Item -Recurse -Force cmake-build-debug -ErrorAction SilentlyContinue
   ```

   Or manually delete the `cmake-build-debug` folder.

2. **In CLion, configure the correct toolchain:**
   - Go to **File → Settings → Build, Execution, Deployment → Toolchains**
   - If using Visual Studio, ensure a Visual Studio toolchain is configured:
     - Click **+** to add a new toolchain
     - Select **Visual Studio**
     - Set path to: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools` (or your Visual Studio installation path)
     - Select **x64** as architecture
   - If using MinGW, ensure MinGW toolchain is properly configured

3. **Reload CMake project:**
   - Go to **File → Reload CMake Project** or press **Ctrl+Shift+O**

4. **If the problem persists, manually configure CMake:**
   ```powershell
   # Remove build directory
   Remove-Item -Recurse -Force cmake-build-debug -ErrorAction SilentlyContinue
   
   # Reconfigure with explicit generator (if needed)
   cmake -B cmake-build-debug -S .
   ```

**Note:** If you have both Visual Studio and MinGW installed, make sure CLion is using the correct toolchain for your project. Visual Studio is recommended for Windows builds.

---

### Build step for curl failed (FetchContent errors)

**Symptoms:**
- Error: "Build step for curl failed: 2"
- Error: "Failed to remove directory: curl-src"
- CMake errors when trying to download/build curl

**Solution:**

1. **Stop any running CMake or build processes:**
   ```powershell
   # Windows PowerShell
   Get-Process | Where-Object {$_.Path -like "*cmake*" -or $_.Path -like "*mingw*" -or $_.Path -like "*curl*"} | Stop-Process -Force -ErrorAction SilentlyContinue
   ```

2. **Clean curl-related directories:**
   ```powershell
   Remove-Item -Recurse -Force cmake-build-debug\_deps\curl-* -ErrorAction SilentlyContinue
   ```

3. **Clean the entire build directory and reconfigure:**
   ```powershell
   Remove-Item -Recurse -Force cmake-build-debug -ErrorAction SilentlyContinue
   cmake -B cmake-build-debug -S .
   ```

4. **If using CLion, reload the CMake project after cleaning**

**Note:** Ensure you have a stable internet connection when CMake downloads dependencies via FetchContent.

---

### CMake Error: Could not find CURL or HTTPS downloads failing

**Symptoms:**
- Error: "Could not find a package configuration file provided by CURL"
- Error: "WARNING: libcurl was compiled without SSL/TLS support. HTTPS downloads will fail."
- HTTPS downloads fail with "Unsupported protocol" error

**Solution:**

If you're using vcpkg to manage dependencies, follow these steps to install curl with SSL support:

1. **Verify vcpkg is installed and working:**
   ```powershell
   # Open a new terminal and test:
   vcpkg version
   ```
   
   If vcpkg is not found, make sure it's in your PATH or navigate to the vcpkg directory.

2. **Remove old curl installation (without SSL):**
   ```powershell
   vcpkg remove curl
   ```
   
   If curl doesn't exist, vcpkg will inform you — this is not a problem.

3. **Install curl with HTTPS support (SSL enabled):**
   
   **Option 1 - Using OpenSSL (recommended):**
   ```powershell
   vcpkg install curl
   ```
   
   This will automatically install curl with OpenSSL support, enabling HTTPS downloads.

4. **Configure CMake with vcpkg toolchain:**
   ```powershell
   cmake -B cmake-build-debug -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```
   
   Replace `C:/vcpkg` with your actual vcpkg installation path.

5. **Rebuild the project:**
   ```powershell
   cmake --build cmake-build-debug
   ```

**Note:** If you're not using vcpkg, you can install libcurl system-wide. On Windows, you can download pre-built binaries or build from source with SSL support.

---

## Common Issues

### Project compiles but executable is not found

**Solution:**

Check the executable location:
- **Linux/macOS**: `build/bin/TemplateBuilder`
- **Windows (Release)**: `build/bin/Release/TemplateBuilder.exe`
- **Windows (Debug)**: `build/bin/Debug/TemplateBuilder.exe`

Ensure you built the project successfully:
```bash
cmake --build build
```

---

### Permission denied when running tests

**Solution:**

Make sure test executables have execute permissions (Linux/macOS):
```bash
chmod +x build/tests/test_*
```

Or run tests directly from the build directory:
```bash
cd build
./tests/test_VariableType
```
