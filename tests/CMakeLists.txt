
cmake_minimum_required(VERSION 3.20)

project(XML_Lib_Unit_Tests VERSION 1.0.0 DESCRIPTION "XML C++ Library Catch Unit Tests" LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

set(TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Unit_Tests)

add_executable(${TEST_EXECUTABLE})
set(TEST_CORE_SOURCES
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
    source/xml/XML_Lib_Tests_XML.cpp
    source/xml/XML_Lib_Tests_Helper.cpp
    source/entitymapper/XML_Lib_Tests_Entity_Mapper.cpp
    source/data/XML_Lib_Tests_External_Reference.cpp
    source/data/XML_Lib_Tests_Value.cpp
    source/data/XML_Lib_Tests_Attribute.cpp
    source/node/XML_Lib_Tests_Variant.cpp
    source/node/XML_Lib_Tests_Comment.cpp
    source/node/XML_Lib_Tests_Content.cpp
    source/node/XML_Lib_Tests_Declaration.cpp
    source/node/XML_Lib_Tests_Element.cpp
    source/node/XML_Lib_Tests_CDATA.cpp
    source/node/XML_Lib_Tests_PI.cpp
    source/node/XML_Lib_Tests_Prolog.cpp
    source/node/XML_Lib_Tests_Root.cpp
    source/node/XML_Lib_Tests_Self.cpp
    source/node/XML_LIb_Tests_EntityReference.cpp
    source/traverse/XML_Lib_Tests_Traverse.cpp
)

if(XML_LIB_ENABLE_STRINGIFY)
    list(APPEND TEST_CORE_SOURCES source/xml/XML_Lib_Tests_Stringify.cpp)
endif()

if(XML_LIB_ENABLE_DTD)
    list(APPEND TEST_CORE_SOURCES
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
        source/node/XML_Lib_Tests_DTD.cpp
    )
endif()

if(XML_LIB_ENABLE_XSD)
    list(APPEND TEST_CORE_SOURCES source/xsd/XML_Lib_Tests_XSD.cpp)
endif()

if(XML_LIB_ENABLE_XPATH)
    list(APPEND TEST_CORE_SOURCES source/xpath/XML_Lib_Tests_XPath.cpp)
endif()

target_sources(${TEST_EXECUTABLE}
    PRIVATE
    ${TEST_CORE_SOURCES}
)
target_include_directories(${TEST_EXECUTABLE}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${Catch2_SOURCE_DIR}/src
)

target_include_directories(${TEST_EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/converter
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/data
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/validator
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/node
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/stringify
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/xpath
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/variants
)

if(NOT XML_LIB_EMBEDDED)
    target_link_libraries(${TEST_EXECUTABLE} PRIVATE ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)
    target_precompile_headers(${TEST_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../stdafx.h)

    add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})
endif()

if(XML_LIB_EMBEDDED)
    set(EMBEDDED_TEST_EXECUTABLE ${XML_LIBRARY_NAME}_Embedded_Unit_Tests)
    add_executable(${EMBEDDED_TEST_EXECUTABLE})
    target_sources(${EMBEDDED_TEST_EXECUTABLE}
        PRIVATE
        source/embedded/XML_Lib_Tests_Embedded.cpp
        source/xml/XML_Lib_Tests_Helper.cpp
    )
    target_include_directories(${EMBEDDED_TEST_EXECUTABLE}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${Catch2_SOURCE_DIR}/src
    )
    target_include_directories(${EMBEDDED_TEST_EXECUTABLE} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../build
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/interface
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/common
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/converter
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/data
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/entity
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/io
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/parser
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/validator
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/node
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/stringify
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/xpath
        ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/variants
    )
    target_link_libraries(${EMBEDDED_TEST_EXECUTABLE} PRIVATE ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)
    target_compile_definitions(${EMBEDDED_TEST_EXECUTABLE} PRIVATE
        $<$<BOOL:${XML_LIB_EMBEDDED}>:XML_LIB_EMBEDDED>
        $<$<BOOL:${XML_LIB_NO_EXCEPTIONS}>:XML_LIB_NO_EXCEPTIONS>
        $<$<BOOL:${XML_LIB_NO_RTTI}>:XML_LIB_NO_RTTI>
        $<$<BOOL:${XML_LIB_MINIMAL_FEATURES}>:XML_LIB_MINIMAL_FEATURES>
        $<$<BOOL:${XML_LIB_ENABLE_XPATH}>:XML_LIB_ENABLE_XPATH>
        $<$<BOOL:${XML_LIB_ENABLE_XSD}>:XML_LIB_ENABLE_XSD>
        $<$<BOOL:${XML_LIB_ENABLE_DTD}>:XML_LIB_ENABLE_DTD>
        $<$<BOOL:${XML_LIB_ENABLE_STRINGIFY}>:XML_LIB_ENABLE_STRINGIFY>
    )
    add_test(NAME ${EMBEDDED_TEST_EXECUTABLE} COMMAND ${EMBEDDED_TEST_EXECUTABLE})
endif()

set(PERFORMANCE_EXECUTABLE ${XML_LIBRARY_NAME}_Performance_Tests)
add_executable(${PERFORMANCE_EXECUTABLE})
target_sources(${PERFORMANCE_EXECUTABLE}
    PRIVATE
    source/performance/XML_Lib_Tests_Performance.cpp
)
target_include_directories(${PERFORMANCE_EXECUTABLE}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${Catch2_SOURCE_DIR}/src
)
target_include_directories(${PERFORMANCE_EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/converter
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/data
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/validator
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/node
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/stringify
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/xpath
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/variants
)
target_link_libraries(${PERFORMANCE_EXECUTABLE} PRIVATE ${XML_LIBRARY_NAME} Catch2::Catch2WithMain)
if(NOT XML_LIB_EMBEDDED)
    target_precompile_headers(${PERFORMANCE_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../stdafx.h)
endif()
add_test(NAME ${PERFORMANCE_EXECUTABLE} COMMAND ${PERFORMANCE_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)