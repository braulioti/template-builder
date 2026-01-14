# Unit Tests Configuration
# This file is included from the main CMakeLists.txt when BUILD_TESTS is enabled

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Use FetchContent to download Google Test
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    # Disable Google Test's own tests and samples
    set(BUILD_GMOCK ON CACHE BOOL "Build gmock")
    set(gtest_build_tests OFF CACHE BOOL "Disable gtest tests")
    set(gtest_build_samples OFF CACHE BOOL "Disable gtest samples")
    set(gtest_build_benchmarks OFF CACHE BOOL "Disable gtest benchmarks")
    set(gtest_force_shared_crt ON CACHE BOOL "Force shared CRT on Windows")
    
    FetchContent_MakeAvailable(googletest)
endif()

# Helper function to add a test executable
function(add_unit_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    target_link_libraries(${TEST_NAME}
        PRIVATE
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            yaml-cpp
    )
    
    # Include directories
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
    )
    
    # Link source files (will need to be adjusted when source files exist)
    # For now, tests will link against the main executable or libraries when created
    
    # Add test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()

# Individual test executables (to be uncommented as tests are written)
# add_unit_test(test_VariableType types/test_VariableType.cpp)
# add_unit_test(test_PromptType types/test_PromptType.cpp)
# add_unit_test(test_FileType types/test_FileType.cpp)
# add_unit_test(test_FileBuilder builders/test_FileBuilder.cpp)
# add_unit_test(test_FolderBuilder builders/test_FolderBuilder.cpp)
# add_unit_test(test_PromptBuilder builders/test_PromptBuilder.cpp)
# add_unit_test(test_ParseYAML services/test_ParseYAML.cpp)

# Message
message(STATUS "Unit tests configuration: Tests will be built when BUILD_TESTS is ON")
message(STATUS "  Test framework: Google Test")
message(STATUS "  Test directory: ${CMAKE_CURRENT_SOURCE_DIR}/tests")