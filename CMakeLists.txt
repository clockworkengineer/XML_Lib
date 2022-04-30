cmake_minimum_required(VERSION 3.23.1)

project("XMLLib" VERSION 0.0.1 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

if(MSVC)
  add_compile_options(/W4 /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-Wall -Werror -pedantic)
endif()

configure_file(XMLConfig.h.in XMLConfig.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

# XML sources and includes

set (XML_SOURCES
    ./classes/XML.cpp
    ./classes/XML_parse.cpp
    ./classes/XML_stringify.cpp
    ./classes/XML_core.cpp
    ./classes/DTD.cpp
    ./classes/DTD_parse.cpp
    ./classes/DTD_parse_content.cpp
    ./classes/DTD_parse_external.cpp
    ./classes/XML_validator.cpp
)
 
set (XML_INCLUDES
    XMLConfig.hpp
    ./include/XML.hpp
    ./include/ISource.hpp
    ./inclide/IDestination.hpp
    ./include/XML_sources.hpp
    ./include/XML_destinations.hpp
    ./include/XML_nodes.hpp
    ./incldue/XML_core.hpp
    ./include/DTD.hpp
    ./include/XML_validator.hpp
)

# XML library

add_library(${XML_LIBRARY_NAME} ${XML_SOURCES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR} )

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)


