cmake_minimum_required(VERSION 3.18.1)

project("XML_Lib Unit Tests" VERSION 1.0.0 DESCRIPTION "XML C++ Library Catch Unit Tests" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0
)

FetchContent_MakeAvailable(Catch2)

# Enable unit tests
enable_testing()

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
        source/io/XML_Lib_Tests_ISource.cpp
        source/io/XML_Lib_Tests_IDestination.cpp
        source/io/XML_Lib_Tests_File.cpp
        source/xml/XML_Lib_Tests_Parse_Misc.cpp
        source/xml/XML_Lib_Tests_Parse_Declaration.cpp
        source/xml/XML_Lib_Tests_Parse_Tags.cpp
        source/xml/XML_Lib_Tests_Parse_Root.cpp
        source/xml/XML_Lib_Tests_Parse_Attributes.cpp
        source/xml/XML_Lib_Tests_Parse_Comments.cpp
        source/xml/XML_Lib_Tests_Parse_Unicode.cpp
        source/xml/XML_Lib_Tests_Parse_Entities.cpp
        source/xml/XML_Lib_Tests_Parse_PI.cpp
        source/xml/XML_Lib_Tests_Parse_CDATA.cpp
        source/xml/XML_Lib_Tests_Parse_Namespace.cpp
        source/xml/XML_Lib_Tests_Stringify.cpp
        source/xml/XML_Lib_Tests_XML.cpp
        source/xml/XML_Lib_Tests_Helper.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Misc.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Entity.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Attributes.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Attributes_Error.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Attributes_Misc.cpp
        source/dtd/XML_Lib_Tests_DTD_Parse_Tags.cpp
        source/dtd/XML_Lib_Tests_DTD_Stringify.cpp
        source/dtd/XML_Lib_Tests_DTD_Validate_Elements.cpp
        source/dtd/XML_Lib_Tests_DTD_Validate_Attributes.cpp
        source/dtd/XML_Lib_Tests_DTD_Validate_Misc.cpp
        source/entitymapper/XML_Lib_Tests_Entity_Mapper.cpp
        source/data/XML_Lib_Tests_External_Reference.cpp
        source/data/XML_Lib_Tests_Value.cpp
        source/data/XML_Lib_Tests_Attribute.cpp
        source/xnode/XML_Lib_Tests_Variant.cpp
        source/xnode/XML_Lib_Tests_Comment.cpp
        source/xnode/XML_Lib_Tests_Content.cpp
        source/xnode/XML_Lib_Tests_Declaration.cpp
        source/xnode/XML_Lib_Tests_Element.cpp
        source/xnode/XML_Lib_Tests_CDATA.cpp
        source/xnode/XML_Lib_Tests_PI.cpp
        source/xnode/XML_Lib_Tests_Prolog.cpp
        source/xnode/XML_Lib_Tests_Root.cpp
        source/xnode/XML_Lib_Tests_Self.cpp
        source/xnode/XML_LIb_Tests_EntityReference.cpp
        source/xnode/XML_Lib_Tests_DTD.cpp
        source/traverse/XML_Lib_Tests_Traverse.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ./include ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)