cmake_minimum_required(VERSION 3.23.1)

project("XMLLib" VERSION 0.1.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

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
    ./classes/implementation/XML_Impl.cpp
    ./classes/implementation/XML_Impl_Parse.cpp
    ./classes/implementation/XML_Impl_Misc.cpp
    ./classes/implementation/XML_Impl_Stringify.cpp
    ./classes/implementation/XML_Core.cpp
    ./classes/DTD.cpp
    ./classes/implementation/DTD_Impl.cpp
    ./classes/implementation/DTD_Impl_Parse.cpp
    ./classes/implementation/DTD_Impl_Parse_Content.cpp
    ./classes/implementation/DTD_Impl_Parse_External.cpp
    ./classes/implementation/XML_Validator.cpp
    ./classes/implementation/XML_EntityMapper.cpp
)
 
set (XML_INCLUDES
    XML_Config.hpp
    ./include/XML.hpp
    ./include/XML_Nodes.hpp
    ./include/XML_Errors.hpp
    ./include/XML_Types.hpp
    ./include/DTD.hpp
    ./include/interface/ISource.hpp
    ./include/interface/IDestination.hpp
    ./include/interface/IXMLEntityMapper.hpp
    ./include/interface/IXMLValidator.hpp
    ./include/implementation/XML_Impl.hpp
    ./include/implementation/XML_Sources.hpp
    ./include/implementation/XML_Destinations.hpp
    ./include/implementation/XML_Core.hpp
    ./include/implementation/DTD_Impl.hpp
    ./include/implementation/XML_Validator.hpp
    ./include/implementation/XML_EntityMapper.hpp
)

# XML library

add_library(${XML_LIBRARY_NAME} ${XML_SOURCES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/interface ${PROJECT_SOURCE_DIR}/include/implementation ${PROJECT_BINARY_DIR} )

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)


