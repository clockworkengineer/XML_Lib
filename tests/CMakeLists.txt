cmake_minimum_required(VERSION 3.18.4)

project("XML_Lib Unit Tests" VERSION 1.0.0 DESCRIPTION "XML C++ Library Catch Unit Tests" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

# Enable unit tests
enable_testing()

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    ./IO/XML_Lib_Tests_ISource.cpp
    ./IO/XML_Lib_Tests_IDestination.cpp
    ./XML/XML_Lib_Tests_Parse_Misc.cpp
    ./XML/XML_Lib_Tests_Parse_Declaration.cpp
    ./XML/XML_Lib_Tests_Parse_Tags.cpp
    ./XML/XML_Lib_Tests_Parse_Root.cpp
    ./XML/XML_Lib_Tests_Parse_Files.cpp
    ./XML/XML_Lib_Tests_Parse_Attributes.cpp
    ./XML/XML_Lib_Tests_Parse_Comments.cpp
    ./XML/XML_Lib_Tests_Parse_Unicode.cpp
    ./XML/XML_Lib_Tests_Parse_Entities.cpp
    ./XML/XML_Lib_Tests_Parse_PI.cpp
    ./XML/XML_Lib_Tests_Parse_CDATA.cpp
    ./XML/XML_Lib_Tests_Parse_Namespace.cpp
    ./XML/XML_Lib_Tests_Stringify.cpp
    ./XML/XML_Lib_Tests_XML.cpp
    ./XML/XML_Lib_Tests_Helper.cpp
    ./DTD/XML_Lib_Tests_DTD_Parse_Misc.cpp
    ./DTD/XML_Lib_Tests_DTD_Parse_Entity.cpp
    ./DTD/XML_Lib_Tests_DTD_Parse_Attributes.cpp
    ./DTD/XML_Lib_Tests_DTD_Parse_Tags.cpp
    ./DTD/XML_Lib_Tests_DTD_Stringify.cpp
    ./DTD/XML_Lib_Tests_DTD_Validate_Elements.cpp
    ./DTD/XML_Lib_Tests_DTD_Validate_Attributes.cpp
    ./DTD/XML_Lib_Tests_DTD_Validate_Misc.cpp
    ./entitymapper/XML_Lib_Tests_Entity_Mapper.cpp
    ./entitymapper/XML_Lib_Tests_External_Reference.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ./include ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/$<CONFIG>/files)