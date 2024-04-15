#pragma once
// ======================
// Test framework include
// ======================
#include "catch2/catch_all.hpp"
// =========================
// Class under test includes
// =========================
#include "XML.hpp"
#include "XML_Core.hpp"
#include "DTD.hpp"
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
#define KEmptyXMLFile "empty.xml"
// ==========================
// Unit test helper functions
// ==========================
std::string prefixPath(const std::string &file);
void checkStringify(const std::string &xmlString);
void verifyCRLFCount(XML_Lib::ISource &source, long lfFinal, long crFinal);