# CMake generated Testfile for 
# Source directory: /home/robt/projects/XML_Lib/tests
# Build directory: /home/robt/projects/XML_Lib/build_embedded/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[XML_Lib_Embedded_Unit_Tests]=] "/home/robt/projects/XML_Lib/build_embedded/tests/XML_Lib_Embedded_Unit_Tests")
set_tests_properties([=[XML_Lib_Embedded_Unit_Tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/robt/projects/XML_Lib/tests/CMakeLists.txt;159;add_test;/home/robt/projects/XML_Lib/tests/CMakeLists.txt;0;")
subdirs("../_deps/catch2-build")
