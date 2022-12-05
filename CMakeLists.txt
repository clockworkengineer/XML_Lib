cmake_minimum_required(VERSION 3.22.1)

project("XML_Lib" VERSION 0.1.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

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
    ./classes/DTD.cpp
    ./classes/implementation/DTD_Impl.cpp
    ./classes/implementation/DTD_Impl_Parse.cpp
    ./classes/implementation/DTD_Impl_Parse_Content.cpp
    ./classes/implementation/DTD_Impl_Parse_External.cpp
    ./classes/implementation/XML_Common.cpp
    ./classes/implementation/XML_Validator.cpp
    ./classes/implementation/XML_EntityMapper.cpp
)
 
set (XML_INCLUDES
    XML_Config.hpp
    ./include/XML.hpp
    ./include/XML_Forward.hpp
    ./include/DTD.hpp
    ./include/DTD_Forward.hpp
    ./include/interface/ISource.hpp
    ./include/interface/IDestination.hpp
    ./include/interface/IEntityMapper.hpp
    ./include/interface/IValidator.hpp
    ./include/implementation/dtd/DTD_Impl.hpp
    ./include/implementation/io/XML_Sources.hpp
    ./include/implementation/io/XML_BufferSource.hpp
    ./include/implementation/io/XML_FileSource.hpp
    ./include/implementation/io/XML_Destinations.hpp
    ./include/implementation/io/XML_BufferDestination.hpp
    ./include/implementation/io/XML_FileDestination.hpp
    ./include/implementation/validator/XML_Validator.hpp
    ./include/implementation/xml/XML_Error.hpp
    ./include/implementation/xml/XML_Value.hpp
    ./include/implementation/xml/XML_Attribute.hpp
    ./include/implementation/xml/XML_ExternalReference.hpp
    ./include/implementation/xml/XML_EntityMapping.hpp
    ./include/implementation/xml/XML_Impl.hpp
    ./include/implementation/xml/XML_Common.hpp
    ./include/implementation/xml/XML_Core.hpp
    ./include/implementation/xml/XML_EntityMapper.hpp
    ./include/implementation/xnode/XML_XNode.hpp
    ./include/implementation/xnode/XML_XProlog.hpp
    ./include/implementation/xnode/XML_XDeclaration.hpp
    ./include/implementation/xnode/XML_XContent.hpp
    ./include/implementation/xnode/XML_XCDATA.hpp
    ./include/implementation/xnode/XML_XEntityReference.hpp
    ./include/implementation/xnode/XML_XElement.hpp
    ./include/implementation/xnode/XML_XComment.hpp
    ./include/implementation/xnode/XML_XPI.hpp
    ./include/implementation/xnode/XML_XDTD.hpp
    ./include/implementation/xnode/XML_Reference.hpp
    ./include/implementation/xnode/XML_Creation.hpp
)

# XML library

add_library(${XML_LIBRARY_NAME} ${XML_SOURCES} ${XML_INCLUDES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/interface
    ${PROJECT_SOURCE_DIR}/include/implementation
    ${PROJECT_SOURCE_DIR}/include/implementation/dtd
    ${PROJECT_SOURCE_DIR}/include/implementation/io
    ${PROJECT_SOURCE_DIR}/include/implementation/validator
    ${PROJECT_SOURCE_DIR}/include/implementation/xml
    ${PROJECT_SOURCE_DIR}/include/implementation/xnode
    ${PROJECT_BINARY_DIR} )

# Add tests

add_subdirectory(tests)

# install

install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)


