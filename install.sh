#!/bin/bash

set -e  # Exit on error

BUILD_PATH="./build/flatpack_cli"
INSTALL_PATH="/usr/local/bin/flatpack"

if [ ! -f "$BUILD_PATH" ]; then
  echo "Error: flatpack_cli binary not found at $BUILD_PATH"
  echo "Please build the project first."
  exit 1
fi

echo "Installing flatpack to $INSTALL_PATH"
sudo cp "$BUILD_PATH" "$INSTALL_PATH"
sudo chmod +x "$INSTALL_PATH"

echo "Installation successful. You can now run 'flatpack' from the terminal."
