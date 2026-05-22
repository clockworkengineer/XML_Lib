#pragma once
#include "common/XML_Error.hpp"

#include "ISource.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

class FileSource final : public ISource
{
public:
  // FileSource Error
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("FileSource");
#endif
  // Constructors/Destructors
  static constexpr std::size_t kMaxSourceBytes{ XML_LIB_MAX_XML_SIZE };

  explicit FileSource(const std::string_view &sourceFileName, std::size_t maxSourceBytes = kMaxSourceBytes) : filename(sourceFileName)
  {
    source.open(sourceFileName.data(), std::ios_base::binary);
    if (!source.is_open()) { XML_LIB_THROW(Error("File input stream failed to open or does not exist.")); }

    source.seekg(0, std::ios_base::end);
    const std::streampos fileSize = source.tellg();
    if (fileSize == static_cast<std::streampos>(-1) || static_cast<std::size_t>(fileSize) > maxSourceBytes) {
      XML_LIB_THROW(Error("File exceeds maximum allowed size."));
    }
    source.seekg(0, std::ios_base::beg);

    if (current_character() == kCarriageReturn) {
      source.get();
      if (current_character() != kLineFeed) { source.unget(); }
    }
  }
  FileSource() = default;
  FileSource(const FileSource &other) = delete;
  FileSource &operator=(const FileSource &other) = delete;
  FileSource(FileSource &&other) = delete;
  FileSource &operator=(FileSource &&other) = delete;
  ~FileSource() noexcept override = default;

  [[nodiscard]] Char current() const override { return current_character(); }
  void next() override
  {
    if (!more()) { XML_LIB_THROW(Error("Parse buffer empty before parse complete.")); }
    source.get();
    if (current() == kCarriageReturn) {
      source.get();
      if (current() != kLineFeed) { source.unget(); }
    }
    columnNo++;
    if (current() == kLineFeed) {
      lineNo++;
      columnNo = 1;
    }
  }
  [[nodiscard]] bool more() const override { return source.peek() != EOF; }
  void backup(const long length) override
  {
    if (static_cast<long>(source.tellg()) - length >= 0 || current() == static_cast<Char>(EOF)) {
      source.clear();
      source.seekg(-length, std::ios_base::cur);
    } else {
      source.seekg(0, std::ios_base::beg);
    }
  }
  [[nodiscard]] long position() const override { return static_cast<long>(source.tellg()); }
  void reset() override
  {
    lineNo = 1;
    columnNo = 1;
    source.clear();
    source.seekg(0, std::ios_base::beg);
  }
  [[nodiscard]] std::string getRange(const long start, const long end) override
  {
    if (start < 0 || end < 0 || end < start) {
      XML_LIB_THROW(Error("Invalid range requested from FileSource."));
    }

    const std::streamsize length = static_cast<std::streamsize>(end - start);
    if (length == 0) {
      return {};
    }

    std::streampos currentPosition = source.tellg();
    if (currentPosition == static_cast<std::streampos>(-1)) {
      source.clear();
    }

    source.seekg(0, std::ios_base::end);
    const std::streampos fileSize = source.tellg();
    if (fileSize == static_cast<std::streampos>(-1) || end > fileSize) {
      source.clear();
      if (currentPosition != static_cast<std::streampos>(-1)) {
        source.seekg(currentPosition, std::ios_base::beg);
      }
      XML_LIB_THROW(Error("Requested range exceeds file size."));
    }

    std::string rangeBuffer;
    rangeBuffer.resize(static_cast<std::size_t>(length));

    source.seekg(start, std::ios_base::beg);
    source.read(rangeBuffer.data(), length);
    if (source.gcount() != length) {
      source.clear();
      if (currentPosition != static_cast<std::streampos>(-1)) {
        source.seekg(currentPosition, std::ios_base::beg);
      }
      XML_LIB_THROW(Error("Failed to read requested range from file."));
    }

    if (currentPosition != static_cast<std::streampos>(-1)) {
      source.clear();
      source.seekg(currentPosition, std::ios_base::beg);
    }

    return rangeBuffer;
  }
  std::string getFileName() { return filename; }
  void close() { source.close(); }

private:
  [[nodiscard]] Char current_character() const { return static_cast<Char>(source.peek()); }
  mutable std::ifstream source;
  std::string filename;
};
}// namespace XML_Lib