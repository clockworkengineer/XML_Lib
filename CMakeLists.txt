cmake_minimum_required(VERSION 3.16.3)

project("XMLLib" VERSION 0.0.1 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

if(MSVC)
  add_compile_options(/W4 /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-Wall -Werror -pedantic)
endif()

configure_file(XML_Config.h.in XML_Config.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

# XML sources and includes

set (XML_SOURCES
    ./classes/XML.cpp
    ./classes/XML_Impl.cpp
    ./classes/XML_Impl_Parse.cpp
    ./classes/XML_Impl_Stringify.cpp
    ./classes/XML_Core.cpp
    ./classes/DTD.cpp
    ./classes/DTD_Parse.cpp
    ./classes/DTD_Parse_Content.cpp
    ./classes/DTD_Parse_External.cpp
    ./classes/XML_Validator.cpp
    ./classes/XML_EntityMapper.cpp
)
 
set (XML_INCLUDES
    XML_Config.hpp
    ./include/interface/ISource.hpp
    ./include/interface/IDestination.hpp
    ./include/interface/IXMLEntityMapper.hpp
    ./include/interface/IXMLValidator.hpp
    ./include/XML.hpp
    ./include/XML_Impl.hpp
    ./include/XML_Sources.hpp
    ./include/XML_Destinations.hpp
    ./include/XML_Nodes.hpp
    ./include/XML_Core.hpp
    ./include/XML_Errors.hpp
    ./include/XML_Types.hpp
    ./include/DTD.hpp
    ./include/DTD_ELement.hpp
    ./include/XML_Validator.hpp
    ./include/XML_EntityMapper.hpp
)

# XML library

add_library(${XML_LIBRARY_NAME} ${XML_SOURCES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/interface ${PROJECT_BINARY_DIR} )

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)


