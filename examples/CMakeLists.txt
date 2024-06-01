cmake_minimum_required(VERSION 3.18.1)

project("XML_Lib Examples" VERSION 1.0.0 DESCRIPTION "XML_Lib example programs")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options

if(MSVC)
    add_compile_options(/W4)
    set(PLATFORM "windows")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall -Werror -pedantic)
    set(PLATFORM "linux")
else()
    add_compile_options(-Wall -Werror -pedantic)
    set(PLATFORM "linux")
endif()

# Get example program list

file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Build each program

foreach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})
    string(REPLACE ".cpp" "" EXAMPLE_TARGET ${EXAMPLE_PROGRAM})
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_PROGRAM}
            include/XML_Utility.hpp)
    target_link_libraries(${EXAMPLE_TARGET} XML_Lib)
    target_include_directories(${EXAMPLE_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/external)
    install(TARGETS ${EXAMPLE_TARGET} DESTINATION bin)
endforeach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})

#add_custom_command(TARGET XML_Parse_File POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files $<CONFIG>/files)