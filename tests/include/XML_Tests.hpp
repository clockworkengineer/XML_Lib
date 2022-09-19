#pragma once
// ======================
// Test framework include
// ======================
#include "catch2/catch_all.hpp"
// =========================
// Class under test includes
// =========================
#include "XML.hpp"
#include "XML_Types.hpp"
#include "IXMLEntityMapper.hpp"
#include "DTD.hpp"
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
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
std::string prefixPath(const std::string &file);
std::string readXMLFromFileUTF8(const std::string &xmlFileName);
std::u16string readXMLFromFileUTF16(const std::string &xmlFileName);
void writeXMLToFileUTF8(const std::string &xmlFileName, const std::string &xmlString);
void writeXMLToFileUTF16(const std::string &xmlFileName, const std::u16string &xmlString, bool be);
void checkStringify(const std::string &xmlString);
void verifyCRLFCount(XML_Lib::ISource &source, long lfFinal, long crFinal);