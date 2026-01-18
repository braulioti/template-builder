# Unit Tests

This directory contains unit tests for the Template Builder project.

## Structure

Tests are organized to match the source code structure:

```
tests/
├── types/          # Tests for type definitions
│   ├── test_VariableType.cpp
│   ├── test_PromptType.cpp
│   └── test_FileType.cpp
├── builders/       # Tests for builder components
│   ├── test_FileBuilder.cpp
│   ├── test_FolderBuilder.cpp
│   └── test_PromptBuilder.cpp
└── services/       # Tests for service components
    └── test_ParseYAML.cpp
```

## Building Tests

Tests are built when the `BUILD_TESTS` option is enabled:

```bash
cmake -DBUILD_TESTS=ON ..
cmake --build .
```

## Running Tests

Run tests using CTest:

```bash
ctest
```

Or run individual test executables:

```bash
./bin/test_VariableType
```

## Test Framework

This project uses **Google Test** (gtest) as the testing framework. Tests are automatically downloaded via CMake's FetchContent if not found on the system.

## Writing Tests

When writing tests, follow these conventions:

1. One test file per source file (e.g., `test_VariableType.cpp` for `VariableType.cpp`)
2. Use descriptive test names: `TEST(ClassName, MethodName_Scenario_ExpectedResult)`
3. Test both positive and negative cases
4. Use appropriate Google Test assertions (`EXPECT_EQ`, `ASSERT_TRUE`, etc.)
5. Test edge cases and error conditions

Example test structure:

```cpp
#include <gtest/gtest.h>
#include "types/VariableType.hpp"

TEST(VariableTest, Constructor_SetsDefaultValues) {
    Variable var;
    EXPECT_EQ(var.GetName(), "");
    EXPECT_EQ(var.GetType(), VariableType::String);
}

TEST(VariableTest, SetValue_StoresValueCorrectly) {
    Variable var;
    var.SetValue("test");
    EXPECT_EQ(var.GetValue(), "test");
}
```