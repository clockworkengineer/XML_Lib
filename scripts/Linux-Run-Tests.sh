 #!/bin/bash
cd build/tests/
./XML_Lib_Unit_Tests
if [ $? -eq 0 ]; then
  cd ../../
else
  exit $?
fi