cmake_minimum_required(VERSION 3.18.1)

project("XML_Lib" VERSION 1.1.0 DESCRIPTION "XML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
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
  ./classes/source/XML.cpp
  ./classes/source/implementation/xml/XML_Impl.cpp
  ./classes/source/implementation/xml/file/XML_File.cpp
  ./classes/source/implementation/xml/parser/XML_Parser.cpp
  ./classes/source/implementation/variant/XML_Variant.cpp
  ./classes/source/implementation/dtd/DTD_Validator.cpp
  ./classes/source/implementation/dtd/DTD_Impl.cpp
  ./classes/source/implementation/dtd/DTD_Impl_Validator.cpp
  ./classes/source/implementation/dtd/DTD_Impl_Parse.cpp
  ./classes/source/implementation/dtd/DTD_Impl_Parse_Content.cpp
  ./classes/source/implementation/dtd/DTD_Impl_Parse_External.cpp
  ./classes/source/implementation/common/XML_Character.cpp
  ./classes/source/implementation/common/XML_Parse.cpp
  ./classes/source/implementation/common/XML_Utility.cpp
  ./classes/source/implementation/entity/XML_EntityMapper.cpp
)

# Platform specific sources
list(APPEND XML_SOURCES "./classes/source/implementation/common/converter/${PLATFORM}/XML_Converter.cpp")

# XML  includes
set(XML_INCLUDES
  XML_Config.hpp
  ./classes/include/XML.hpp
  ./classes/include/DTD_Validator.hpp
  ./classes/include/interface/ISource.hpp
  ./classes/include/interface/IDestination.hpp
  ./classes/include/interface/IEntityMapper.hpp
  ./classes/include/interface/IValidator.hpp
  ./classes/include/interface/IParser.hpp
  ./classes/include/interface/IStringify.hpp
  ./classes/include/interface/XML_Interfaces.hpp
  ./classes/include/implementation/XML_Core.hpp
  ./classes/include/implementation/common/XML_Character.hpp
  ./classes/include/implementation/common/XML_Converter.hpp
  ./classes/include/implementation/common/XML_Error.hpp
  ./classes/include/implementation/common/XML_Parse.hpp
  ./classes/include/implementation/common/XML_Utility.hpp
  ./classes/include/implementation/data/XML_Value.hpp
  ./classes/include/implementation/data/XML_Attribute.hpp
  ./classes/include/implementation/data/XML_ExternalReference.hpp
  ./classes/include/implementation/DTD_Impl.hpp
  ./classes/include/implementation/entity/XML_EntityMapper.hpp
  ./classes/include/implementation/io/XML_Sources.hpp
  ./classes/include/implementation/io/XML_BufferSource.hpp
  ./classes/include/implementation/io/XML_FileSource.hpp
  ./classes/include/implementation/io/XML_Destinations.hpp
  ./classes/include/implementation/io/XML_BufferDestination.hpp
  ./classes/include/implementation/io/XML_FileDestination.hpp
  ./classes/include/implementation/XML_Impl.hpp
  ./classes/include/implementation/parser/XML_Parser.hpp
  ./classes/include/implementation/stringify/XML_Stringify.hpp
  ./classes/include/implementation/xnode/XML_XNode.hpp
  ./classes/include/implementation/xnode/XML_Reference.hpp
  ./classes/include/implementation/xnode/XML_Creation.hpp
  ./classes/include/implementation/xnode/XML_Index.hpp
  ./classes/include/implementation/variants/XML_Prolog.hpp
  ./classes/include/implementation/variants/XML_Declaration.hpp
  ./classes/include/implementation/variants/XML_Content.hpp
  ./classes/include/implementation/variants/XML_CDATA.hpp
  ./classes/include/implementation/variants/XML_EntityReference.hpp
  ./classes/include/implementation/variants/XML_Element.hpp
  ./classes/include/implementation/variants/XML_Root.hpp
  ./classes/include/implementation/variants/XML_Self.hpp
  ./classes/include/implementation/variants/XML_Comment.hpp
  ./classes/include/implementation/variants/XML_PI.hpp
  ./classes/include/implementation/variants/XML_DTD.hpp
  ./classes/include/implementation/variants/XML_Variant.hpp
  ./classes/include/interface/IAction.hpp
  ./classes/include/implementation/xnode/XML_Contents.hpp
  ./classes/include/implementation/entity/XML_EntityMapping.hpp
)

# XML library
add_library(${XML_LIBRARY_NAME} ${XML_SOURCES} ${XML_INCLUDES})
target_include_directories(${XML_LIBRARY_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/classes/include
  ${PROJECT_SOURCE_DIR}/classes/include/interface
  ${PROJECT_SOURCE_DIR}/classes/include/implementation
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/common
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/data
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/entity
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/io
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/parser
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/validator
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/xnode
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/stringify
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/variants
  ${PROJECT_BINARY_DIR})

# Add tests
add_subdirectory(tests)

# Add examples
add_subdirectory(examples)

# install
install(TARGETS ${XML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${XML_INCLUDES} DESTINATION include)
