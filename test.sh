#!/bin/bash

set -e  # Exit on error

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Build directory does not exist. Run build.sh first."
    exit 1
fi

cd build

# Check if test executable exists
if [ ! -f "./archive_test" ]; then
    echo "Test executable not found. Build the project first."
    exit 1
fi

# Run the test
./archive_test