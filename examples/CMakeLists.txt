
cmake_minimum_required(VERSION 3.20)

project(XML_Lib_Examples VERSION 1.2.0 DESCRIPTION "XML_Lib example programs")

file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/ "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

foreach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})
  get_filename_component(EXE_NAME ${EXAMPLE_PROGRAM} NAME)
  add_executable(${EXE_NAME})
  target_sources(${EXE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/${EXAMPLE_PROGRAM})
  target_precompile_headers(${EXE_NAME} REUSE_FROM XML_Lib)
  target_include_directories(${EXE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/external)
  target_include_directories(${EXE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/build
    ${CMAKE_SOURCE_DIR}/classes/include/interface
    ${CMAKE_SOURCE_DIR}/classes/include/implementation
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/common
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/converter
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/data
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/entity
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/io
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/parser
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/validator
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/node
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/stringify
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/xpath
    ${CMAKE_SOURCE_DIR}/classes/include/implementation/variants
  )
  target_link_libraries(${EXE_NAME} PRIVATE XML_Lib)
  install(TARGETS ${EXE_NAME} DESTINATION bin)
endforeach()

add_custom_command(TARGET XML_Parse_File.cpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)