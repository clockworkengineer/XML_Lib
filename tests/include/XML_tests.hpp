#pragma once
// ======================
// Test framework include
// ======================
#include "catch2/catch.hpp"
// =========================
// Class under test includes
// =========================
#include "XML.hpp"
#include "XML_sources.hpp"
#include "XML_destinations.hpp"
// ============
// STL includes
// ============
#include <fstream>
#include <filesystem>
// ===================
// Unit test constants
// ===================
#define kGeneratedXMLFile "generated.xml"
#define kSingleXMLFile "testfile001.xml"
#define kNonExistantXMLFile "doesntexist.xml"
// ==========================
// Unit test helper functions
// ==========================
std::string prefixTestDataPath(const std::string &file);
std::string readXMLFromFileUTF8(const std::string &xmlFileName);
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName);
void writeXMLToFileUTF8(const std::string &xmlFileName, const std::string &xmlString);
void writeXMLToFileUTF16(const std::string &xmlFileName, const std::u16string &xmlString, bool be);
void checkStringify(const std::string &xmlString);
void verifyCRLFCount(XMLLib::ISource &source, long lfFinal, long crFinal);