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
    "testfile001.xml",               \
    "testfile002.xml",               \
    "testfile003.xml",               \
    "testfile005.xml",               \
    "testfile007.xml",               \
    "testfile010.xml",               \
    "testfile011.xml",               \
    "testfile012.xml",               \
    "testfile013.xml",               \
    "testfile014.xml",               \
    "testfile015.xml",               \
  }));

std::string prefixTestDataPath(const std::string &xmlFileName);
void checkStringify(const std::string &xmlString);
void verifyCRLFCount(XML_Lib::ISource &source, long lfFinal, long crFinal);
std::string generateRandomFileName(void );

using namespace XML_Lib;
