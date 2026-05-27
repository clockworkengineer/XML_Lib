#!/bin/bash

set -euo pipefail

 cd ./Release/tests/
if [ ! -x ./XML_Lib_Performance_Tests ]; then
  echo "Performance test binary not found: ./build/tests/XML_Lib_Performance_Tests"
  exit 1
fi
./XML_Lib_Performance_Tests
