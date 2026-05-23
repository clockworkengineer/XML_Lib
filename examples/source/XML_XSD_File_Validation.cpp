// Program: XML_XSD_File_Validation
//
// Description: Demonstrates loading both an XML document and an XSD schema
// from files and validating the document against the schema. Mirrors the
// file-based workflow of XML_Parse_File but with XSD validation.
//
// Dependencies: C++20, XML_Lib.
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
  std::cout << "Validating  " << xmlPath.filename().string() << "  against  " << xsdPath.filename().string() << std::endl;
  try {
    xl::XML xml;
    xml.parse(xl::FileSource{ xmlPath.string() });
    const std::string schema = readFile(xsdPath);
    xml.validate(schema);
    std::cout << "  -> Passed." << std::endl;
  } catch (const xl::IValidator::Error &e) {
    std::cerr << "  -> Validation error: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "  -> Error: " << e.what() << std::endl;
  }
}

// ---- Main ------------------------------------------------------------------

/// <summary>Program entry point.</summary>
int main()
{
  std::cout << "XML_XSD_File_Validation started ..." << std::endl;
  const fs::path filesDir = fs::current_path() / "files";

  // Validate every .xml file in files/ against a matching .xsd if one exists
  for (const auto &entry : fs::directory_iterator(filesDir)) {
    if (entry.path().extension() != ".xml") { continue; }
    fs::path xsdPath = entry.path();
    xsdPath.replace_extension(".xsd");
    if (!fs::exists(xsdPath)) {
      std::cout << entry.path().filename().string() << "  — no matching .xsd, skipping." << std::endl;
      continue;
    }
    validatePair(entry.path(), xsdPath);
  }

  std::cout << "XML_XSD_File_Validation exited." << std::endl;
  return EXIT_SUCCESS;
}
