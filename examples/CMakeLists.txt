cmake_minimum_required(VERSION 3.18.1)

project("XML_Lib Examples" VERSION 1.1.0 DESCRIPTION "XML_Lib example programs")

# Get example program list

file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/ "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# Build each program

foreach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})
    string(REPLACE ".cpp" "" EXAMPLE_TARGET ${EXAMPLE_PROGRAM})
    add_executable(${EXAMPLE_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/source/${EXAMPLE_PROGRAM})
    target_link_libraries(${EXAMPLE_TARGET} XML_Lib)
    target_include_directories(${EXAMPLE_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/external)
    target_precompile_headers(${EXAMPLE_TARGET} REUSE_FROM XML_Lib)
    install(TARGETS ${EXAMPLE_TARGET} DESTINATION bin)
endforeach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})

add_custom_command(TARGET XML_Parse_File POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)