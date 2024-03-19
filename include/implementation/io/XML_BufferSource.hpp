#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <type_traits>

#include "ISource.hpp"

namespace XML_Lib {

class BufferSource : public ISource
{
public:
  // Bits per byte
  static const int kBitsPerByte{ 8 };

  // Constructors/Destructors
  explicit BufferSource(const std::u16string &sourceBuffer)// UTF16 source BE/LE
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
    std::u16string utf16xml{ sourceBuffer };
    if (utf16xml.find(u"<?xml") != 0) {
      for (char16_t &ch : utf16xml) {
        ch = (static_cast<uint16_t>(ch) >> kBitsPerByte) | (static_cast<uint16_t>(ch) << kBitsPerByte);
      }
    }
#if defined(U16)
    buffer = utf16xml;
#else defined(U32)
    buffer = converter.toUtf32(utf16xml);
#endif
    convertCRLFToLF(buffer);
  }
  explicit BufferSource(const std::string &sourceBuffer)
  {
    if (sourceBuffer.empty()) { throw Error("Empty source buffer passed to be parsed."); }
#if defined(U16)
    buffer = converter.toUtf16(sourceBuffer);
#else
    buffer = converter.toUtf32(sourceBuffer);
#endif
    convertCRLFToLF(buffer);
  }
  BufferSource() = default;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() = default;

  [[nodiscard]] XML_Lib::Char current() const override
  {
    if (more()) { return (buffer[bufferPosition]); }
    return (static_cast<XML_Lib::Char>(EOF));
  }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    bufferPosition++;
    columnNo++;
    if (current() == kLineFeed) {
      lineNo++;
      columnNo = 1;
    }
  }
  [[nodiscard]] bool more() const override { return (bufferPosition < static_cast<long>(buffer.size())); }
  void backup(long length) override
  {
    bufferPosition -= length;
    if (bufferPosition < 0) { bufferPosition = 0; }
  }
  [[nodiscard]] long position() const override { return (bufferPosition); }
  std::string getRange(long start, long end) override
  {
    return (converter.toUtf8(buffer.substr(start, static_cast<std::size_t>(end) - start)));
  }
  void reset() override
  {
    lineNo = 1;
    columnNo = 1;
    bufferPosition = 0;
  }

private:
  void convertCRLFToLF(XML_Lib::String &xmlString)
  {
    size_t pos = xmlString.find(U"\x0D\x0A");
    while (pos != std::string::npos) {
      xmlString.replace(pos, 2, U"\x0A");
      pos = xmlString.find(U"\x0D\x0A", pos + 1);
    }
  }

  long bufferPosition = 0;
  XML_Lib::String buffer;
};
}// namespace XML_Lib