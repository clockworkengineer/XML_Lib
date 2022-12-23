#pragma once
// =======
// C++ STL
// =======
#include <fstream>
#include <string>
#include <stdexcept>
// ================
// Source interface
// ================
#include "ISource.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class BufferSource : public ISource
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // Bits per byte
  static const int kBitsPerByte{ 8 };
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit BufferSource(const std::u16string &sourceBuffer)// UTF16 source BE/LE
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
    std::u16string utf16xml{ sourceBuffer };
    if (utf16xml.find(u"<?xml") != 0) {
      for (char16_t &ch : utf16xml) {
        ch = (static_cast<uint16_t>(ch) >> kBitsPerByte) | (static_cast<uint16_t>(ch) << kBitsPerByte);
      }
    }
    m_buffer = toUtf32(utf16xml);
    convertCRLFToLF(m_buffer);
  }
  explicit BufferSource(const std::string &sourceBuffer)
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
    m_buffer = toUtf32(sourceBuffer);
    convertCRLFToLF(m_buffer);
  }
  BufferSource() = default;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() = default;
  // ==============
  // PUBLIC METHODS
  // ==============
  [[nodiscard]] XML_Lib::Char current() const override
  {
    if (more()) { return (m_buffer[m_position]); }
    return (static_cast<XML_Lib::Char>(EOF));
  }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    m_position++;
    m_column++;
    if (current() == kLineFeed) {
      m_lineNo++;
      m_column = 1;
    }
  }
  [[nodiscard]] bool more() const override { return (m_position < static_cast<long>(m_buffer.size())); }
  void backup(long length) override
  {
    m_position -= length;
    if (m_position < 0) { m_position = 0; }
  }
  [[nodiscard]] long position() const override { return (m_position); }
  std::string getRange(long start, long end) override
  {
    return (toUtf8(m_buffer.substr(start, static_cast<std::size_t>(end) - start)));
  }
  void reset() override
  {
    m_lineNo = 1;
    m_column = 1;
    m_position = 0;
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
  void convertCRLFToLF(XML_Lib::String &xmlString)
  {
    size_t pos = xmlString.find(U"\x0D\x0A");
    while (pos != std::string::npos) {
      xmlString.replace(pos, 2, U"\x0A");
      pos = xmlString.find(U"\x0D\x0A", pos + 1);
    }
  }
  // =================
  // PRIVATE VARIABLES
  // =================
  long m_position = 0;
  XML_Lib::String m_buffer;
};
}// namespace XML_Lib