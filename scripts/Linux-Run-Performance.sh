#!/bin/bash

set -euo pipefail

cd ./Release/tests/
if [ ! -x ./XML_Lib_Performance_Tests ]; then
  echo "Release performance test binary not found: ./Release/tests/XML_Lib_Performance_Tests"
  exit 1
fi
./XML_Lib_Performance_Tests
cd ../../

cd ./Debug/tests/
if [ ! -x ./XML_Lib_Performance_Tests ]; then
  echo "Debug performance test binary not found: ./Debug/tests/XML_Lib_Performance_Tests"
  exit 1
fi
./XML_Lib_Performance_Tests
