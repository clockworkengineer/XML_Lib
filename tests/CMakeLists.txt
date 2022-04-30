cmake_minimum_required(VERSION 3.16.0)

project("XML Unit Tests" VERSION 0.0.1 DESCRIPTION "XML C++ Library Catch Unit Tests" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.4
)

FetchContent_MakeAvailable(Catch2)

# Enable unit tests

enable_testing()

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
    ./IO/ISource.cpp
    ./IO/IDestination.cpp
    ./XML/XML_parse_misc.cpp
    ./XML/XML_parse_declaration.cpp
    ./XML/XML_parse_tags.cpp
    ./XML/XML_parse_root.cpp
    ./XML/XML_parse_files.cpp
    ./XML/XML_parse_attributes.cpp
    ./XML/XML_parse_comments.cpp
    ./XML/XML_parse_unicode.cpp
    ./XML/XML_parse_entities.cpp
    ./XML/XML_parse_pi.cpp
    ./XML/XML_parse_cdata.cpp
    ./XML/XML_parse_namespace.cpp
    ./XML/XML_stringify.cpp
    ./XML/XML_helper.cpp
    ./DTD/DTD_parse_misc.cpp
    ./DTD/DTD_parse_entity.cpp
    ./DTD/DTD_parse_attributes.cpp
    ./DTD/DTD_parse_tags.cpp
    ./DTD/DTD_stringify.cpp
    ./DTD/DTD_validate_elements.cpp
    ./DTD/DTD_validate_attributes.cpp
)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ./include ../include)
target_link_libraries(${TEST_EXECUTABLE} ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/testData testData)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/testData ${PROJECT_BINARY_DIR}/$<CONFIG>/testData)