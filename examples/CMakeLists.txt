
cmake_minimum_required(VERSION 3.20)

project(XML_Lib_Examples VERSION 1.3.0 DESCRIPTION "XML_Lib example programs")

option(XML_LIB_INSTALL_EXAMPLES "Install example binaries" OFF)

file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/ "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

foreach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})
  get_filename_component(EXE_NAME ${EXAMPLE_PROGRAM} NAME_WLE)
  add_executable(${EXE_NAME})
  target_sources(${EXE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/${EXAMPLE_PROGRAM})
  if(MSVC)
    target_precompile_headers(${EXE_NAME} REUSE_FROM XML_Lib)
  endif()
  target_include_directories(${EXE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  target_link_libraries(${EXE_NAME} PRIVATE XML_Lib)
  if(XML_LIB_INSTALL_EXAMPLES)
    install(TARGETS ${EXE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif()
endforeach()

if(TARGET XML_Parse_File)
  add_custom_command(TARGET XML_Parse_File POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)
endif()