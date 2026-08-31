#pragma once
#include "XML.hpp"
#include "XML_Converter.hpp"
#include "common/XML_Error.hpp"
#include "common/XML_LineColumnTracker.hpp"
#include "common/XML_StreamNormalizer.hpp"
#include "common/XML_Utility.hpp"

#include "interface/ISource.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

class BufferSource final : public ISource
{
public:
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("BufferSource");
#endif
  static constexpr std::size_t kMaxSourceBytes{ XML_LIB_MAX_XML_SIZE };

  explicit BufferSource(const std::u16string_view &sourceBuffer,
    std::size_t maxSourceBytes = kMaxSourceBytes,
    std::string_view systemId = "<buffer>")
    : systemId(systemId)
  {
    if (sourceBuffer.empty()) { XML_LIB_THROW(Error("Empty source buffer passed to be parsed.")); }
    if (static_cast<std::size_t>(sourceBuffer.size()) > maxSourceBytes / sizeof(char16_t)) {
      XML_LIB_THROW(Error("Source buffer exceeds maximum allowed size."));
    }
    std::u16string utf16xml{ sourceBuffer };
    StreamNormalizer::swapUTF16ByteOrderIfNeeded(utf16xml);
    StreamNormalizer::stripBOM(utf16xml);
    buffer = utf16xml;
    StreamNormalizer::convertCRLFToLF(buffer);
  }

  explicit BufferSource(const std::string_view &sourceBuffer,
    std::size_t maxSourceBytes = kMaxSourceBytes,
    std::string_view systemId = "<buffer>")
    : systemId(systemId)
  {
    if (sourceBuffer.empty()) { XML_LIB_THROW(Error("Empty source buffer passed to be parsed.")); }
    if (sourceBuffer.size() > maxSourceBytes) {
      XML_LIB_THROW(Error("Source buffer exceeds maximum allowed size."));
    }
    buffer = toUtf16(std::string(sourceBuffer));
    StreamNormalizer::stripBOM(buffer);
    StreamNormalizer::convertCRLFToLF(buffer);
  }

  BufferSource() = default;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() noexcept override = default;

  [[nodiscard]] Char current() const override
  {
    if (more()) { return buffer[bufferPosition]; }
    return static_cast<Char>(EOF);
  }

  void next() override
  {
    if (!more()) { XML_LIB_THROW(Error("Parse buffer empty before parse complete.")); }
    bufferPosition++;
    tracker.advance(current(), bufferPosition);
  }

  [[nodiscard]] bool more() const override { return bufferPosition < static_cast<long>(buffer.size()); }

  void backup(const long length) override
  {
    bufferPosition -= length;
    if (bufferPosition < 0) { bufferPosition = 0; }
    tracker.rewindTo(bufferPosition);
  }

  [[nodiscard]] long position() const override { return bufferPosition; }

  [[nodiscard]] std::pair<long, long> getPosition() const override { return tracker.getPosition(); }

  [[nodiscard]] std::string getSystemId() const override { return systemId; }

  [[nodiscard]] std::string getRange(const long start, const long end) override
  {
    if (start < 0 || end < 0 || end < start) {
      XML_LIB_THROW(Error("Invalid range requested from BufferSource."));
    }
    const auto size = static_cast<std::size_t>(buffer.size());
    if (static_cast<std::size_t>(end) > size) {
      XML_LIB_THROW(Error("Requested range exceeds source buffer size."));
    }
    return toUtf8(buffer.substr(start, static_cast<std::size_t>(end) - start));
  }

  void reset() override
  {
    bufferPosition = 0;
    tracker.reset();
  }

private:
  long bufferPosition = 0;
  String buffer;
  std::string systemId = "<buffer>";
  LineColumnTracker tracker;
};

}// namespace XML_Lib