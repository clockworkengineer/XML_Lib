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
  explicit BufferSource(const std::u16string &sourceBuffer) 
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
    m_parseBuffer = sourceBuffer;
    if (m_parseBuffer.find(u"<?xml") != 0) {
      for (char16_t &ch : m_parseBuffer) {
        ch = (static_cast<uint16_t>(ch) >> kBitsPerByte) | (static_cast<uint16_t>(ch) << kBitsPerByte);
      }
    }
    convertCRLFToLF(m_parseBuffer);
  }
  explicit BufferSource(const std::string &sourceBuffer)
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
    m_parseBuffer = m_UTF8.from_bytes(sourceBuffer);
    convertCRLFToLF(m_parseBuffer);
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
  [[nodiscard]] ISource::Char current() const override
  {
    if (more()) { return (m_parseBuffer[m_bufferPosition]); }
    return (static_cast<ISource::Char>(EOF));
  }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    m_bufferPosition++;
    m_column++;
    if (current() == kLineFeed) {
      m_lineNo++;
      m_column = 1;
    }
  }
  [[nodiscard]] bool more() const override { return (m_bufferPosition < static_cast<long>(m_parseBuffer.size())); }
  void backup(long length) override
  {
    m_bufferPosition -= length;
    if (m_bufferPosition < 0) { m_bufferPosition = 0; }
  }
  [[nodiscard]] long position() const override { return (m_bufferPosition); }
  std::string getRange(long start, long end) override
  {
    return (m_UTF8.to_bytes(m_parseBuffer.substr(start, static_cast<std::size_t>(end) - start)));
  }
  void reset() override
  {
    m_lineNo = 1;
    m_column = 1;
    m_bufferPosition = 0;
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
  void convertCRLFToLF(ISource::String &xmlString)
  {
    size_t pos = xmlString.find(u"\x0D\x0A");
    while (pos != std::string::npos) {
      xmlString.replace(pos, 2, u"\x0A");
      pos = xmlString.find(u"\x0D\x0A", pos + 1);
    }
  }
  // =================
  // PRIVATE VARIABLES
  // =================
  long m_bufferPosition = 0;
  ISource::String m_parseBuffer;
};
}// namespace XML_Lib