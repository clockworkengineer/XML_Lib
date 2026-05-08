# Modernized CMakeLists.txt for XML_Lib
cmake_minimum_required(VERSION 3.20)

project(XML_Lib VERSION 1.2.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(XML_LIB_ENABLE_LTO "Enable link-time optimization" ON)
option(XML_LIB_BUILD_SIZE_OPTIMIZED "Build with size optimization" OFF)
option(XML_LIB_BUILD_TESTS "Build unit tests" ON)
option(XML_LIB_BUILD_EXAMPLES "Build example programs" ON)
option(XML_LIB_EMBEDDED "Enable embedded-friendly build defaults" OFF)
option(XML_LIB_NO_EXCEPTIONS "Disable exception support for embedded targets" OFF)
option(XML_LIB_STRICT_NO_EXCEPTIONS "Compile with -fno-exceptions only when the codebase supports it" OFF)
option(XML_LIB_NO_RTTI "Disable RTTI support for embedded targets" OFF)
option(XML_LIB_MINIMAL_FEATURES "Enable minimal embedded feature subset" OFF)
option(XML_LIB_ENABLE_XPATH "Enable XPath engine" ON)
option(XML_LIB_ENABLE_XSD "Enable XSD validation support" ON)
option(XML_LIB_ENABLE_DTD "Enable DTD validation support" ON)
option(XML_LIB_ENABLE_STRINGIFY "Enable XML stringify support" ON)
option(XML_LIB_ENABLE_PERFORMANCE_COUNTERS "Enable internal performance counters" OFF)

if(XML_LIB_EMBEDDED)
  set(XML_LIB_BUILD_SIZE_OPTIMIZED ON CACHE BOOL "Build with size optimization" FORCE)
  set(XML_LIB_NO_EXCEPTIONS ON CACHE BOOL "Disable exception support for embedded targets" FORCE)
  set(XML_LIB_STRICT_NO_EXCEPTIONS ON CACHE BOOL "Compile with -fno-exceptions only when the codebase supports it" FORCE)
  set(XML_LIB_NO_RTTI ON CACHE BOOL "Disable RTTI support for embedded targets" FORCE)
  set(XML_LIB_MINIMAL_FEATURES ON CACHE BOOL "Enable minimal embedded feature subset" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  if(XML_LIB_EMBEDDED)
    set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "Choose the type of build." FORCE)
  else()
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  endif()
endif()

if(XML_LIB_MINIMAL_FEATURES)
  set(XML_LIB_ENABLE_XPATH OFF CACHE BOOL "Enable XPath engine" FORCE)
  set(XML_LIB_ENABLE_XSD OFF CACHE BOOL "Enable XSD validation support" FORCE)
  set(XML_LIB_ENABLE_DTD OFF CACHE BOOL "Enable DTD validation support" FORCE)
  set(XML_LIB_ENABLE_STRINGIFY ON CACHE BOOL "Enable XML stringify support" FORCE)
endif()

if(MSVC)
  add_compile_options(/W4)
  set(PLATFORM "windows")
else()
  add_compile_options(-Wall -Werror -pedantic -fvisibility=hidden)
  set(PLATFORM "linux")
endif()

configure_file(XML_Config.h.in XML_Config.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

set(XML_LIBRARY_SOURCES
  classes/source/XML.cpp
  classes/source/implementation/xml/XML_Impl.cpp
  classes/source/implementation/xml/file/XML_File.cpp
  classes/source/implementation/xml/parser/Default_Parser.cpp
  classes/source/implementation/variant/XML_Variant.cpp
  classes/source/implementation/common/XML_Character.cpp
  classes/source/implementation/common/XML_Parse.cpp
  classes/source/implementation/common/XML_Utility.cpp
  classes/source/implementation/entity/XML_EntityMapper.cpp
  classes/source/implementation/converter/${PLATFORM}/XML_Converter.cpp
)

if(XML_LIB_ENABLE_DTD)
  list(APPEND XML_LIBRARY_SOURCES
    classes/source/implementation/dtd/DTD_Validator.cpp
    classes/source/implementation/dtd/DTD_Validator_Impl.cpp
    classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp
    classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp
    classes/source/implementation/dtd/DTD_Validator_Impl_Parse_Content.cpp
    classes/source/implementation/dtd/DTD_Validator_Impl_Parse_External.cpp
  )
endif()

if(XML_LIB_ENABLE_XSD)
  list(APPEND XML_LIBRARY_SOURCES
    classes/source/implementation/xsd/XSD_Validator.cpp
    classes/source/implementation/xsd/XSD_Validator_Impl.cpp
    classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp
    classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
  )
endif()

if(XML_LIB_ENABLE_XPATH)
  list(APPEND XML_LIBRARY_SOURCES
    classes/source/XPath.cpp
    classes/source/implementation/xpath/XPath_Lexer.cpp
    classes/source/implementation/xpath/XPath_Parser.cpp
    classes/source/implementation/xpath/XPath_Evaluator.cpp
    classes/source/implementation/xpath/XPath_Impl.cpp
  )
endif()

add_library(${XML_LIBRARY_NAME})

target_sources(${XML_LIBRARY_NAME}
  PRIVATE
  ${XML_LIBRARY_SOURCES}
)

target_include_directories(${XML_LIBRARY_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include>
  PRIVATE
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
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/xpath>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/classes/include/implementation/variants>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_compile_definitions(${XML_LIBRARY_NAME}
  PRIVATE XML_LIB_INTERNAL
  PUBLIC
  $<$<BOOL:${XML_LIB_EMBEDDED}>:XML_LIB_EMBEDDED>
  $<$<BOOL:${XML_LIB_NO_EXCEPTIONS}>:XML_LIB_NO_EXCEPTIONS>
  $<$<BOOL:${XML_LIB_STRICT_NO_EXCEPTIONS}>:XML_LIB_STRICT_NO_EXCEPTIONS>
  $<$<BOOL:${XML_LIB_NO_RTTI}>:XML_LIB_NO_RTTI>
  $<$<BOOL:${XML_LIB_MINIMAL_FEATURES}>:XML_LIB_MINIMAL_FEATURES>
  $<$<BOOL:${XML_LIB_BUILD_SIZE_OPTIMIZED}>:XML_LIB_BUILD_SIZE_OPTIMIZED>
  $<$<BOOL:${XML_LIB_ENABLE_XPATH}>:XML_LIB_ENABLE_XPATH>
  $<$<BOOL:${XML_LIB_ENABLE_XSD}>:XML_LIB_ENABLE_XSD>
  $<$<BOOL:${XML_LIB_ENABLE_DTD}>:XML_LIB_ENABLE_DTD>
  $<$<BOOL:${XML_LIB_ENABLE_STRINGIFY}>:XML_LIB_ENABLE_STRINGIFY>
  $<$<BOOL:${XML_LIB_ENABLE_PERFORMANCE_COUNTERS}>:XML_LIB_ENABLE_PERFORMANCE_COUNTERS>
)

if(NOT MSVC)
  if(XML_LIB_BUILD_SIZE_OPTIMIZED)
    target_compile_options(${XML_LIBRARY_NAME} PRIVATE
      $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-Os>
    )
  else()
    target_compile_options(${XML_LIBRARY_NAME} PRIVATE
      $<$<CONFIG:Release>:-O3>
      $<$<CONFIG:MinSizeRel>:-Os>
    )
  endif()

  if(XML_LIB_ENABLE_LTO)
    target_compile_options(${XML_LIBRARY_NAME} PRIVATE
      $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-flto>
    )
  endif()

  if(XML_LIB_NO_RTTI)
    target_compile_options(${XML_LIBRARY_NAME} PRIVATE -fno-rtti)
  endif()

  if(XML_LIB_NO_EXCEPTIONS OR XML_LIB_STRICT_NO_EXCEPTIONS)
    if(MSVC)
      target_compile_options(${XML_LIBRARY_NAME} PRIVATE /EHs-c-)
    else()
      target_compile_options(${XML_LIBRARY_NAME} PRIVATE -fno-exceptions)
    endif()
  endif()

  if(XML_LIB_EMBEDDED)
    target_compile_options(${XML_LIBRARY_NAME} PRIVATE
      -ffunction-sections
      -fdata-sections
    )
    target_link_options(${XML_LIBRARY_NAME} PRIVATE -Wl,--gc-sections)
  endif()
endif()

if(NOT XML_LIB_EMBEDDED)
  target_precompile_headers(${XML_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)
endif()

set(XML_PUBLIC_HEADERS
  ${PROJECT_SOURCE_DIR}/classes/include/XML.hpp
  ${PROJECT_SOURCE_DIR}/classes/include/XML_Sources.hpp
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/io/XML_BufferSource.hpp
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/io/XML_FileSource.hpp
  ${PROJECT_SOURCE_DIR}/classes/include/interface/XML_Interfaces.hpp
)

if(XML_LIB_ENABLE_XPATH)
  list(APPEND XML_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/classes/include/XPath.hpp)
endif()

if(XML_LIB_ENABLE_DTD)
  list(APPEND XML_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/classes/include/DTD_Validator.hpp)
endif()

if(XML_LIB_ENABLE_XSD)
  list(APPEND XML_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/classes/include/XSD_Validator.hpp)
endif()

if(XML_LIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(XML_LIB_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

find_program(SIZE_EXECUTABLE NAMES size)

if(SIZE_EXECUTABLE)
  add_custom_target(size_report
    COMMAND ${CMAKE_COMMAND} -E echo "Library size report for ${XML_LIBRARY_NAME}:"
    COMMAND ${SIZE_EXECUTABLE} --format=gnu $<TARGET_FILE:${XML_LIBRARY_NAME}>
    DEPENDS ${XML_LIBRARY_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_target(size_report_embedded
    COMMAND ${CMAKE_COMMAND} -E echo "Embedded library size report for ${XML_LIBRARY_NAME}:"
    COMMAND ${SIZE_EXECUTABLE} --format=gnu $<TARGET_FILE:${XML_LIBRARY_NAME}>
    DEPENDS ${XML_LIBRARY_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

# install
install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_PUBLIC_HEADERS} DESTINATION include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/classes/include/interface DESTINATION include
  FILES_MATCHING PATTERN "*.hpp"
)
