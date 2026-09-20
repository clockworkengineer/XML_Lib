#!/bin/bash
set -euo pipefail

echo "=== Running Release Unit Tests ==="
ctest --test-dir Release --output-on-failure -R XML_Lib_Unit_Tests

echo "=== Running Debug Unit Tests ==="
ctest --test-dir Debug --output-on-failure -R XML_Lib_Unit_Tests