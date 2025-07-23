#!/bin/bash

set -e  # Stop on error

# Create build directory if it does not exist
mkdir -p build
rm -rf build/*
cd build

# Configure project
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Build project
cmake --build . # > build_log.txt 2>&1