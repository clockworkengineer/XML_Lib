#pragma once

#include "catch2/catch_all.hpp"

#include "XML.hpp"
#include "XML_Core.hpp"
#include "DTD_Validator.hpp"

#include <fstream>
#include <filesystem>

#define kGeneratedXMLFile "generated.xml"
#define kSingleXMLFile "testfile001.xml"
#define kNonExistantXMLFile "doesntexist.xml"
#define KEmptyXMLFile "empty.xml"

#define TEST_FILE_LIST(file)                 \
  auto file = GENERATE(values<std::string>({ \
    "./files/testfile001.xml",               \
    "./files/testfile002.xml",               \
    "./files/testfile003.xml",               \
    "./files/testfile005.xml",               \
    "./files/testfile007.xml",               \
    "./files/testfile010.xml",               \
    "./files/testfile011.xml",               \
    "./files/testfile012.xml",               \
    "./files/testfile013.xml",               \
    "./files/testfile014.xml",               \
    "./files/testfile015.xml",               \
  }));

std::string prefixPath(const std::string &file);
void checkStringify(const std::string &xmlString);
void verifyCRLFCount(XML_Lib::ISource &source, long lfFinal, long crFinal);

using namespace XML_Lib;
