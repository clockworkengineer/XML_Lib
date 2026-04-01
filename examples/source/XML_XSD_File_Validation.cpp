// Program: XML_XSD_File_Validation
//
// Description: Demonstrates loading both an XML document and an XSD schema
// from files and validating the document against the schema. Mirrors the
// file-based workflow of XML_Parse_File but with XSD validation.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;
namespace fs = std::filesystem;

// ---- Helpers ---------------------------------------------------------------

/// Read the entire contents of a text file into a std::string.
static std::string readFile(const fs::path &path)
{
  std::ifstream file{ path };
  if (!file) { throw std::runtime_error("Cannot open file: " + path.string()); }
  return { std::istreambuf_iterator<char>{ file }, {} };
}

/// Parse an XML file and validate it against a schema file, logging the outcome.
static void validatePair(const fs::path &xmlPath, const fs::path &xsdPath)
{
  PLOG_INFO << "Validating  " << xmlPath.filename().string()
            << "  against  " << xsdPath.filename().string();
  try {
    xl::XML xml;
    xml.parse(xl::FileSource{ xmlPath.string() });
    const std::string schema = readFile(xsdPath);
    xml.validate(schema);
    PLOG_INFO << "  -> Passed.";
  } catch (const xl::IValidator::Error &e) {
    PLOG_WARNING << "  -> Validation error: " << e.what();
  } catch (const std::exception &e) {
    PLOG_ERROR << "  -> Error: " << e.what();
  }
}

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XSD_File_Validation.log");
  PLOG_INFO << "XML_XSD_File_Validation started ...";

  const fs::path filesDir = fs::current_path() / "files";

  // Validate every .xml file in files/ against a matching .xsd if one exists
  for (const auto &entry : fs::directory_iterator(filesDir)) {
    if (entry.path().extension() != ".xml") { continue; }
    fs::path xsdPath = entry.path();
    xsdPath.replace_extension(".xsd");
    if (!fs::exists(xsdPath)) {
      PLOG_INFO << entry.path().filename().string() << "  — no matching .xsd, skipping.";
      continue;
    }
    validatePair(entry.path(), xsdPath);
  }

  PLOG_INFO << "XML_XSD_File_Validation exited.";
  return EXIT_SUCCESS;
}
