#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <stdexcept>
#include <fstream>
// =====================
// Destination interface
// =====================
#include "IDestination.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class FileDestination : public IDestination
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit FileDestination(const std::string &destinationFileName)
  {
    m_destination.open(destinationFileName.c_str(), std::ios_base::binary);
    if (!m_destination.is_open()) { throw Error("File output stream failed to open or could not be created."); }
  }
  FileDestination() = default;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() = default;
  // ==============
  // PUBLIC METHODS
  // ==============
  void add(ISource::Char c) override { add(m_UTF8.to_bytes(c)); }
  void add(const std::string &bytes) override
  {
    m_destination.write(bytes.c_str(), static_cast<std::streamsize>(bytes.length()));
    m_destination.flush();
  }
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===============
  // PRIVATE METHODS
  // ===============
  // =================
  // PRIVATE VARIABLES
  // =================
  std::ofstream m_destination;
};
}// namespace XML_Lib