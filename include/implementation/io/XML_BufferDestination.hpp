#pragma once

#include <string>
#include <stdexcept>
#include <fstream>
// =====================
// Destination interface
// =====================
#include "IDestination.hpp"

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
    for (auto b : bytes) { m_buffer.push_back(b); }
  }
  void add(const XML_Lib::Char ch) override { add(toUtf8(ch)); }
  [[nodiscard]] std::string getBuffer() const { return (m_buffer); }
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
  std::string m_buffer;
};
}// namespace XML_Lib