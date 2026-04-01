# Modernized CMakeLists.txt for XML_Lib
cmake_minimum_required(VERSION 3.20)

project(XML_Lib VERSION 1.1.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4)
  set(PLATFORM "windows")
else()
  add_compile_options(-Wall -Werror -pedantic)
  set(PLATFORM "linux")
endif()

configure_file(XML_Config.h.in XML_Config.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

add_library(${XML_LIBRARY_NAME})

target_sources(${XML_LIBRARY_NAME}
  PRIVATE
  classes/source/XML.cpp
  classes/source/implementation/xml/XML_Impl.cpp
  classes/source/implementation/xml/file/XML_File.cpp
  classes/source/implementation/xml/parser/Default_Parser.cpp
  classes/source/implementation/variant/XML_Variant.cpp
  classes/source/implementation/dtd/DTD_Validator.cpp
  classes/source/implementation/dtd/DTD_Validator_Impl.cpp
  classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp
  classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp
  classes/source/implementation/dtd/DTD_Validator_Impl_Parse_Content.cpp
  classes/source/implementation/dtd/DTD_Validator_Impl_Parse_External.cpp
  classes/source/implementation/xsd/XSD_Validator.cpp
  classes/source/implementation/xsd/XSD_Validator_Impl.cpp
  classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp
  classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
  classes/source/implementation/common/XML_Character.cpp
  classes/source/implementation/common/XML_Parse.cpp
  classes/source/implementation/common/XML_Utility.cpp
  classes/source/implementation/entity/XML_EntityMapper.cpp
  classes/source/implementation/converter/${PLATFORM}/XML_Converter.cpp
  XML_Config.hpp
)

target_include_directories(${XML_LIBRARY_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/interface>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/common>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/converter>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/data>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/entity>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/io>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/parser>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/validator>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/node>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/stringify>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/variants>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_precompile_headers(${XML_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

add_subdirectory(tests)
add_subdirectory(examples)

# install
install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)
