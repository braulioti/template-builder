# Unit Tests Configuration
# This file is included from the main CMakeLists.txt when BUILD_TESTS is enabled

# Enable testing
enable_testing()

# Include FetchContent before using it
include(FetchContent)

# Find Google Test first (might be installed system-wide)
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Use FetchContent to download Google Test
    message(STATUS "Google Test not found, downloading via FetchContent...")
    
    # Set options BEFORE declaring and making available
    set(BUILD_GMOCK ON CACHE BOOL "Build gmock")
    set(gtest_build_tests OFF CACHE BOOL "Disable gtest tests")
    set(gtest_build_samples OFF CACHE BOOL "Disable gtest samples")
    set(gtest_build_benchmarks OFF CACHE BOOL "Disable gtest benchmarks")
    set(gtest_force_shared_crt ON CACHE BOOL "Force shared CRT on Windows")
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(googletest)

    # Verify that GTest targets are available
    # Google Test provides both the old-style targets (gtest, gtest_main)
    # and the new-style imported targets (GTest::gtest, GTest::gtest_main)
    if(TARGET GTest::gtest AND TARGET GTest::gtest_main)
        message(STATUS "Google Test downloaded and configured successfully")
    elseif(TARGET gtest AND TARGET gtest_main)
        message(STATUS "Google Test downloaded (using legacy targets)")
    else()
        message(FATAL_ERROR "Failed to configure Google Test - targets not found")
    endif()
else()
    message(STATUS "Using system-installed Google Test")
endif()

# Helper function to add a test executable
function(add_unit_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Determine which GTest targets to use (check inside function to access parent scope variables)
    # Try modern imported targets first, fall back to legacy targets
    if(TARGET GTest::gtest)
        set(GTEST_TARGET GTest::gtest)
        set(GTEST_MAIN_TARGET GTest::gtest_main)
        if(TARGET GTest::gmock)
            set(GMOCK_TARGET GTest::gmock)
        else()
            set(GMOCK_TARGET GTest::gtest)  # Fallback if gmock not available
        endif()
    elseif(TARGET gtest)
        set(GTEST_TARGET gtest)
        set(GTEST_MAIN_TARGET gtest_main)
        if(TARGET gmock)
            set(GMOCK_TARGET gmock)
        else()
            set(GMOCK_TARGET gtest)
        endif()
    else()
        message(FATAL_ERROR "Google Test targets not found for ${TEST_NAME}. Please check GTest configuration.")
    endif()

    # Apply coverage flags if they are set in CMAKE_CXX_FLAGS
    # This ensures coverage data is generated when tests run
    if(CMAKE_CXX_FLAGS MATCHES "coverage|--coverage|-fprofile-arcs|-ftest-coverage")
        target_compile_options(${TEST_NAME} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    endif()

    target_link_libraries(${TEST_NAME}
        PRIVATE
            ${GTEST_TARGET}
            ${GTEST_MAIN_TARGET}
            ${GMOCK_TARGET}
            yaml-cpp
            CURL::libcurl
    )
    
    # Link source files needed for the test
    if(${TEST_NAME} STREQUAL "test_VariableType")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/types/VariableType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_PromptType")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/types/PromptType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/VariableType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_FileType")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/types/FileType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/PromptType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/VariableType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_template-builder")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/template-builder.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_PromptBuilder")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/builders/PromptBuilder.cpp
            ${CMAKE_SOURCE_DIR}/src/types/PromptType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/VariableType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_FileBuilder")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/builders/FileBuilder.cpp
            ${CMAKE_SOURCE_DIR}/src/builders/PromptBuilder.cpp
            ${CMAKE_SOURCE_DIR}/src/types/FileType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/PromptType.cpp
            ${CMAKE_SOURCE_DIR}/src/types/VariableType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_FolderBuilder")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/builders/FolderBuilder.cpp
            ${CMAKE_SOURCE_DIR}/src/types/FileType.cpp
        )
    elseif(${TEST_NAME} STREQUAL "test_RemoteFileBuilder")
        target_sources(${TEST_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/builders/RemoteFileBuilder.cpp
            ${CMAKE_SOURCE_DIR}/src/types/RemoteFileType.cpp
        )
    endif()

    # Include directories
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
    )
    
    # Link source files (will need to be adjusted when source files exist)
    # For now, tests will link against the main executable or libraries when created
    
    # Add test to CTest
    # Use generator expressions to handle both single-config and multi-config generators
    # For single-config generators (Unix Makefiles, Ninja): $<TARGET_FILE:target>
    # For multi-config generators (Visual Studio): $<TARGET_FILE:target> works with -C flag
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Individual test executables (to be uncommented as tests are written)
add_unit_test(test_VariableType test_VariableType.cpp)
add_unit_test(test_PromptType test_PromptType.cpp)
add_unit_test(test_FileType test_FileType.cpp)
add_unit_test(test_template-builder test_template-builder.cpp)
add_unit_test(test_PromptBuilder test_PromptBuilder.cpp)
add_unit_test(test_FileBuilder test_FileBuilder.cpp)
add_unit_test(test_FolderBuilder test_FolderBuilder.cpp)
add_unit_test(test_RemoteFileBuilder test_RemoteFileBuilder.cpp)
# add_unit_test(test_ParseYAML services/test_ParseYAML.cpp)

# Message
message(STATUS "Unit tests configuration: Tests will be built when BUILD_TESTS is ON")
message(STATUS "  Test framework: Google Test")
message(STATUS "  Test directory: ${CMAKE_CURRENT_SOURCE_DIR}/tests")
