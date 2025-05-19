cmake_minimum_required(VERSION 3.14)

project(flatpack-archive
    VERSION 1.0.0
    DESCRIPTION "Flatpack Archive - A lightweight custom compression archive"
    LANGUAGES CXX
)

# Extract version parts
set(FLATPACK_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(FLATPACK_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(FLATPACK_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compression libraries
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# Add library target
add_library(flatpack_archive STATIC
    src/archive.cpp
    src/file_table.cpp
    src/file_entry.cpp
    src/header.cpp
    src/compression.cpp
)

# Include directories (for public headers)
target_include_directories(flatpack_archive
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Define version macros for users
target_compile_definitions(flatpack_archive
    PUBLIC
    FLATPACK_VERSION_MAJOR=${FLATPACK_VERSION_MAJOR}
    FLATPACK_VERSION_MINOR=${FLATPACK_VERSION_MINOR}
    FLATPACK_VERSION_PATCH=${FLATPACK_VERSION_PATCH}
)

# Link compression libraries to flatpack_archive
target_link_libraries(flatpack_archive
    PRIVATE
    z
    lz4
    zstd
    brotlienc
    brotlidec
    brotlicommon
)

# Add test executable
add_executable(archive_test
    tests/test.cpp
)

# Link the flatpack_archive library to the test
target_link_libraries(archive_test
    PRIVATE
    flatpack_archive
)

# Enable testing
enable_testing()

# Add test to ctest
add_test(NAME FlatpackTest COMMAND archive_test)

# Flatpack Cli
add_executable(flatpack_cli tools/cli.cpp)
target_include_directories(flatpack_cli PRIVATE include)
target_link_libraries(flatpack_cli
    PRIVATE
    flatpack_archive
    z
    lz4
    zstd
    brotlienc
    brotlidec
    brotlicommon
)
