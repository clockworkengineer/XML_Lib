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
class BufferDestination : public IDestination
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() = default;
  // ==============
  // PUBLIC METHODS
  // ==============
  void add(const std::string &bytes) override
  {
    for (auto b : bytes) { m_stringifyBuffer.push_back(b); }
  }
  void add(const ISource::Char ch) override { add(m_UTF8.to_bytes(ch)); }
  [[nodiscard]] std::string getBuffer() const { return (m_stringifyBuffer); }
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
  std::string m_stringifyBuffer;
};
}// namespace XML_Lib