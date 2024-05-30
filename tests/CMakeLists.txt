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
        io/XML_Lib_Tests_ISource.cpp
        io/XML_Lib_Tests_IDestination.cpp
        xml/XML_Lib_Tests_Parse_Misc.cpp
        xml/XML_Lib_Tests_Parse_Declaration.cpp
        xml/XML_Lib_Tests_Parse_Tags.cpp
        xml/XML_Lib_Tests_Parse_Root.cpp
        xml/XML_Lib_Tests_Parse_Files.cpp
        xml/XML_Lib_Tests_Parse_Attributes.cpp
        xml/XML_Lib_Tests_Parse_Comments.cpp
        xml/XML_Lib_Tests_Parse_Unicode.cpp
        xml/XML_Lib_Tests_Parse_Entities.cpp
        xml/XML_Lib_Tests_Parse_PI.cpp
        xml/XML_Lib_Tests_Parse_CDATA.cpp
        xml/XML_Lib_Tests_Parse_Namespace.cpp
        xml/XML_Lib_Tests_Stringify.cpp
        xml/XML_Lib_Tests_XML.cpp
        xml/XML_Lib_Tests_Helper.cpp
        dtd/XML_Lib_Tests_DTD_Parse_Misc.cpp
        dtd/XML_Lib_Tests_DTD_Parse_Entity.cpp
        dtd/XML_Lib_Tests_DTD_Parse_Attributes.cpp
        dtd/XML_Lib_Tests_DTD_Parse_Tags.cpp
        dtd/XML_Lib_Tests_DTD_Stringify.cpp
        dtd/XML_Lib_Tests_DTD_Validate_Elements.cpp
        dtd/XML_Lib_Tests_DTD_Validate_Attributes.cpp
        dtd/XML_Lib_Tests_DTD_Validate_Misc.cpp
    ./entitymapper/XML_Lib_Tests_Entity_Mapper.cpp
    ./data/XML_Lib_Tests_External_Reference.cpp
    ./data/XML_Lib_Tests_Value.cpp
    ./data/XML_Lib_Tests_Attribute.cpp
        xnode/XML_Lib_Tests_Variant.cpp
        xnode/XML_Lib_Tests_XComment.cpp
        xnode/XML_Lib_Tests_XContent.cpp
        xnode/XML_Lib_Tests_XDeclaration.cpp
        xnode/XML_Lib_Tests_XElement.cpp
        xnode/XML_Lib_Tests_XCDATA.cpp
        xnode/XML_Lib_Tests_XPI.cpp
        xnode/XML_Lib_Tests_XProlog.cpp
        xnode/XML_Lib_Tests_XRoot.cpp
        xnode/XML_Lib_Tests_XSelf.cpp
        xnode/XML_LIb_Tests_XEntityReference.cpp
        xnode/XML_Lib_Tests_XDTD.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ./include ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/$<CONFIG>/files)