 #!/bin/bash
 cd ./Release/tests/
 ./XML_Lib_Unit_Tests
if [ $? -eq 0 ]; then
  cd ../../
else
  exit $?
fi
 cd ./Debug/tests/
 ./XML_Lib_Unit_Tests
if [ $? -eq 0 ]; then
  cd ../../
else
  exit $?
fi