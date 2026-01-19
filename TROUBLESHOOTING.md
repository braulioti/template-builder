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
