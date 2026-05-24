#!/bin/bash

set -euo pipefail

cd ./Release/tests/
./XML_Lib_Unit_Tests -c "[Compliance]"
