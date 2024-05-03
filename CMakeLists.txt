cmake_minimum_required(VERSION 3.18.4)

project("XML_Lib" VERSION 1.0.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

configure_file(XML_Config.h.in XML_Config.hpp)

set(XML_LIBRARY_NAME ${PROJECT_NAME})

# XML sources
set(XML_SOURCES
  ./classes/XML.cpp
  ./classes/implementation/xml/XML_Impl.cpp
  ./classes/implementation/xml/XML_Impl_File.cpp
  ./classes/implementation/xml/XML_Impl_Parser.cpp
  ./classes/implementation/xml/XML_Impl_Stringify.cpp
  ./classes/implementation/xml/XML_Impl_Validator.cpp
  ./classes/implementation/variant/XML_Variant.cpp
  ./classes/DTD.cpp
  ./classes/implementation/dtd/DTD_Impl.cpp
  ./classes/implementation/dtd/DTD_Impl_Validator.cpp
  ./classes/implementation/dtd/DTD_Impl_Parse.cpp
  ./classes/implementation/dtd/DTD_Impl_Parse_Content.cpp
  ./classes/implementation/dtd/DTD_Impl_Parse_External.cpp
  ./classes/implementation/common/XML_Character.cpp
  ./classes/implementation/common/XML_Parse.cpp
  ./classes/implementation/common/XML_Utility.cpp

  ./classes/implementation/entity/XML_EntityMapper.cpp
)

# Platform specific sources
list(APPEND XML_SOURCES "./classes/implementation/common/converter/${PLATFORM}/XML_Converter.cpp")

# XML  includes
set(XML_INCLUDES
  XML_Config.hpp
  ./include/XML.hpp
  ./include/DTD.hpp
  ./include/interface/ISource.hpp
  ./include/interface/IDestination.hpp
  ./include/interface/IEntityMapper.hpp
  ./include/interface/IValidator.hpp
  ./include/interface/IParser.hpp
  ./include/interface/IStringify.hpp
  ./include/interface/XML_Interfaces.hpp
  ./include/implementation/XML_Core.hpp
  ./include/implementation/common/XML_Character.hpp
  ./include/implementation/common/XML_Converter.hpp
  ./include/implementation/common/XML_Error.hpp
  ./include/implementation/common/XML_Parser.hpp
  ./include/implementation/common/XML_Utility.hpp
  ./include/implementation/data/XML_Value.hpp
  ./include/implementation/data/XML_Attribute.hpp
  ./include/implementation/data/XML_ExternalReference.hpp
  ./include/implementation/DTD_Impl.hpp
  ./include/implementation/entity/XML_EntityMapper.hpp
  ./include/implementation/io/XML_Sources.hpp
  ./include/implementation/io/XML_BufferSource.hpp
  ./include/implementation/io/XML_FileSource.hpp
  ./include/implementation/io/XML_Destinations.hpp
  ./include/implementation/io/XML_BufferDestination.hpp
  ./include/implementation/io/XML_FileDestination.hpp
  ./include/implementation/XML_Impl.hpp
  ./include/implementation/xnode/XML_XNode.hpp
  ./include/implementation/xnode/XML_Reference.hpp
  ./include/implementation/xnode/XML_Creation.hpp
  ./include/implementation/xnode/XML_Index.hpp
  ./include/implementation/variants/XML_XProlog.hpp
  ./include/implementation/variants/XML_XDeclaration.hpp
  ./include/implementation/variants/XML_XContent.hpp
  ./include/implementation/variants/XML_XCDATA.hpp
  ./include/implementation/variants/XML_XEntityReference.hpp
  ./include/implementation/variants/XML_XElement.hpp
  ./include/implementation/variants/XML_XRoot.hpp
  ./include/implementation/variants/XML_XSelf.hpp
  ./include/implementation/variants/XML_XComment.hpp
  ./include/implementation/variants/XML_XPI.hpp
  ./include/implementation/variants/XML_XDTD.hpp
  ./include/implementation/variants/XML_Variant.hpp
)

# XML library
add_library(${XML_LIBRARY_NAME} ${XML_SOURCES} ${XML_INCLUDES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/interface
  ${PROJECT_SOURCE_DIR}/include/implementation
  ${PROJECT_SOURCE_DIR}/include/implementation/common
  ${PROJECT_SOURCE_DIR}/include/implementation/data
  ${PROJECT_SOURCE_DIR}/include/implementation/entity
  ${PROJECT_SOURCE_DIR}/include/implementation/io
  ${PROJECT_SOURCE_DIR}/include/implementation/validator
  ${PROJECT_SOURCE_DIR}/include/implementation/xnode
  ${PROJECT_SOURCE_DIR}/include/implementation/variants
  ${PROJECT_BINARY_DIR})

# Add tests
add_subdirectory(tests)

# install
install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)
