#pragma once

#include "XML.hpp"
#include "common/XML_Error.hpp"
#include "common/XML_LineColumnTracker.hpp"
#include "converter/XML_Converter.hpp"
#include "interface/ISource.hpp"

#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace XML_Lib {

/// @brief Memory-mapped file character stream (zero-copy OS page cache source).
///
/// Maps a file directly into the process address space without copying file data
/// into intermediate userspace heap buffers. Supports random access and streaming
/// XML parsing across POSIX and Windows.
class MMapSource final : public ISource
{
public:
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("MMapSource");
#endif
  static constexpr std::size_t kMaxSourceBytes{ XML_LIB_MAX_XML_SIZE };

  explicit MMapSource(const std::string_view &filePath, std::size_t maxSourceBytes = kMaxSourceBytes)
    : filename(filePath)
  {
    if (filePath.empty()) {
      XML_LIB_THROW(Error("Empty file path passed to MMapSource."));
    }

#if defined(_WIN32)
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
      XML_LIB_THROW(Error("Failed to open file for memory mapping: " + filename));
    }

    LARGE_INTEGER liSize;
    if (!GetFileSizeEx(hFile, &liSize)) {
      CloseHandle(hFile);
      XML_LIB_THROW(Error("Failed to query file size: " + filename));
    }

    fileBytes = static_cast<std::size_t>(liSize.QuadPart);
    if (fileBytes > maxSourceBytes) {
      CloseHandle(hFile);
      XML_LIB_THROW(Error("File exceeds maximum allowed size: " + filename));
    }

    if (fileBytes > 0) {
      HANDLE hMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
      if (!hMapping) {
        CloseHandle(hFile);
        XML_LIB_THROW(Error("Failed to create file mapping: " + filename));
      }

      mappedData = static_cast<const char *>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
      CloseHandle(hMapping);
      CloseHandle(hFile);

      if (!mappedData) {
        XML_LIB_THROW(Error("Failed to map view of file: " + filename));
      }
    } else {
      CloseHandle(hFile);
      mappedData = nullptr;
    }
#else
    int fd = ::open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
      XML_LIB_THROW(Error("Failed to open file for memory mapping: " + filename));
    }

    struct stat sb{};
    if (::fstat(fd, &sb) == -1) {
      ::close(fd);
      XML_LIB_THROW(Error("Failed to query file size: " + filename));
    }

    fileBytes = static_cast<std::size_t>(sb.st_size);
    if (fileBytes > maxSourceBytes) {
      ::close(fd);
      XML_LIB_THROW(Error("File exceeds maximum allowed size: " + filename));
    }

    if (fileBytes > 0) {
      void *addr = ::mmap(nullptr, fileBytes, PROT_READ, MAP_SHARED, fd, 0);
      ::close(fd);

      if (addr == MAP_FAILED) {
        XML_LIB_THROW(Error("Failed to mmap file: " + filename));
      }

#if defined(MADV_SEQUENTIAL)
      ::madvise(addr, fileBytes, MADV_SEQUENTIAL);
#endif
      mappedData = static_cast<const char *>(addr);
    } else {
      ::close(fd);
      mappedData = nullptr;
    }
#endif

    // Convert raw UTF-8 / UTF-16 content into normalized u16string buffer for ISource operations
    if (fileBytes > 0 && mappedData != nullptr) {
      buffer = toUtf16(std::string_view(mappedData, fileBytes));
      // Normalize line endings
      std::u16string normalized;
      normalized.reserve(buffer.size());
      for (std::size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i] == kCarriageReturn) {
          normalized.push_back(kLineFeed);
          if (i + 1 < buffer.size() && buffer[i + 1] == kLineFeed) {
            ++i;
          }
        } else {
          normalized.push_back(buffer[i]);
        }
      }
      buffer = std::move(normalized);
    }
  }

  MMapSource(const MMapSource &) = delete;
  MMapSource &operator=(const MMapSource &) = delete;

  MMapSource(MMapSource &&other) noexcept
    : filename(std::move(other.filename))
    , mappedData(other.mappedData)
    , fileBytes(other.fileBytes)
    , buffer(std::move(other.buffer))
    , cursor(other.cursor)
    , tracker(other.tracker)
  {
    other.mappedData = nullptr;
    other.fileBytes = 0;
    other.cursor = 0;
  }

  MMapSource &operator=(MMapSource &&other) noexcept
  {
    if (this != &other) {
      unmap();
      filename = std::move(other.filename);
      mappedData = other.mappedData;
      fileBytes = other.fileBytes;
      buffer = std::move(other.buffer);
      cursor = other.cursor;
      tracker = other.tracker;

      other.mappedData = nullptr;
      other.fileBytes = 0;
      other.cursor = 0;
    }
    return *this;
  }

  ~MMapSource() noexcept override
  {
    unmap();
  }

  /// @brief Direct zero-copy string view into the mapped file bytes in the OS page cache.
  [[nodiscard]] std::string_view stringView() const noexcept
  {
    return (mappedData != nullptr && fileBytes > 0) ? std::string_view(mappedData, fileBytes) : std::string_view{};
  }

  /// @brief Raw pointer to the mapped file memory.
  [[nodiscard]] const char *data() const noexcept { return mappedData; }

  /// @brief Total size in bytes of the mapped file.
  [[nodiscard]] std::size_t size() const noexcept { return fileBytes; }

  // ISource interface implementations
  [[nodiscard]] Char current() const override
  {
    if (cursor < buffer.size()) { return buffer[cursor]; }
    return 0;
  }

  void next() override
  {
    if (!more()) { XML_LIB_THROW(Error("Parse buffer empty before parse complete.")); }
    tracker.advance(buffer[cursor], static_cast<long>(cursor));
    ++cursor;
  }

  [[nodiscard]] bool more() const override { return cursor < buffer.size(); }

  void backup(long length) override
  {
    if (length < 0) { return; }
    const auto uLen = static_cast<std::size_t>(length);
    if (cursor < uLen) { cursor = 0; } else { cursor -= uLen; }
    tracker.rewindTo(static_cast<long>(cursor));
  }

  void reset() override
  {
    cursor = 0;
    tracker.reset();
  }

  [[nodiscard]] long position() const override { return static_cast<long>(cursor); }

  [[nodiscard]] std::pair<long, long> getPosition() const override { return tracker.getPosition(); }

  [[nodiscard]] std::string getSystemId() const override { return filename; }

  std::string getRange(long start, long end) override
  {
    if (start < 0 || end < start) { return {}; }
    const auto uStart = static_cast<std::size_t>(start);
    const auto uEnd = static_cast<std::size_t>(end);
    if (uStart >= buffer.size()) { return {}; }
    const auto length = std::min(uEnd - uStart, buffer.size() - uStart);
    return toUtf8(std::u16string_view(buffer.data() + uStart, length));
  }

private:
  void unmap() noexcept
  {
    if (mappedData != nullptr && fileBytes > 0) {
#if defined(_WIN32)
      UnmapViewOfFile(mappedData);
#else
      ::munmap(const_cast<char *>(mappedData), fileBytes);
#endif
      mappedData = nullptr;
      fileBytes = 0;
    }
  }

  std::string filename;
  const char *mappedData{ nullptr };
  std::size_t fileBytes{ 0 };
  std::u16string buffer;
  std::size_t cursor{ 0 };
  LineColumnTracker tracker{};
};

} // namespace XML_Lib
