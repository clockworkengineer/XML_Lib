#!/bin/bash
set -euo pipefail

echo "=== Building Release configuration ==="
cmake -S . -B Release -DCMAKE_BUILD_TYPE=Release -DXML_LIB_BUILD_TESTS=ON -DXML_LIB_BUILD_EXAMPLES=ON
cmake --build Release -j$(nproc)

echo "=== Building Debug configuration ==="
cmake -S . -B Debug -DCMAKE_BUILD_TYPE=Debug -DXML_LIB_BUILD_TESTS=ON -DXML_LIB_BUILD_EXAMPLES=ON
cmake --build Debug -j$(nproc)