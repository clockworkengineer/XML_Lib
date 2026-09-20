#!/bin/bash
set -euo pipefail

echo "=== Running Compliance Suite ==="
./Release/tests/XML_Lib_Unit_Tests -c "[Compliance]"
