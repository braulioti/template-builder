cmake_minimum_required(VERSION 3.10)

project(TemplateBuilder
    VERSION 0.1.0
    DESCRIPTION "Generate project templates using YAML files"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
option(BUILD_TESTS "Build unit tests" OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+
    if(MSVC)
        # Set runtime library
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
elseif(APPLE)
    # Handle universal binaries if needed (x64 + ARM64)
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "") 
        # Default to native architecture
        set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR})
    endif()
endif()

# Find yaml-cpp
find_package(yaml-cpp QUIET)

if(NOT yaml-cpp_FOUND)
    # If yaml-cpp is not found, use FetchContent to download it
    include(FetchContent)
    
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests")
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable yaml-cpp tools")
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "Disable yaml-cpp contrib")
    
    FetchContent_MakeAvailable(yaml-cpp)
    set(yaml-cpp_VERSION "master (latest)")
else()
    set(yaml-cpp_VERSION ${yaml-cpp_VERSION})
endif()

# Source files
# Note: Additional source files will be added as the code is converted from Pascal
set(SOURCES
    src/template-builder.cpp
)

# Headers
# Note: Header files will be added as the code is converted from Pascal
set(HEADERS
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Platform-specific compiler options (applied to target)
# Note: Generator expressions only work with multi-config generators (like Visual Studio)
# For single-config generators (Makefiles, Ninja), CMake handles Debug/Release flags automatically
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Od>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/O2>)
else()
    # For single-config generators, CMake automatically uses CMAKE_CXX_FLAGS_<CONFIG>
    # Just add the common flags here
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        yaml-cpp
)

# C++17 filesystem library (required on some compilers)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(${PROJECT_NAME} PRIVATE c++fs)
endif()

# Unit Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install documentation (if needed)
install(FILES README.md
    DESTINATION share/doc/${PROJECT_NAME}
    OPTIONAL
)

# Display configuration summary
message(STATUS "")
message(STATUS "Template Builder Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
if(APPLE)
    message(STATUS "  Architecture: ${CMAKE_OSX_ARCHITECTURES}")
else()
    message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  yaml-cpp: ${yaml-cpp_VERSION}")
message(STATUS "")
