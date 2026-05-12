#pragma once

#include "XML_Types.hpp"
#include <cwctype>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace XML_Lib {
// ===================
// Character constants
// ===================
constexpr char kCarriageReturn{ 0x0D };
constexpr char kLineFeed{ 0x0A };

/// @brief Abstract interface for a readable character stream used during XML/DTD parsing.
///
/// Implementations (`BufferSource`, `FileSource`) wrap a string or file and expose a
/// cursor-based character-by-character API.  The parser reads through `current()` / `next()`
/// and can `backup()` when it needs to un-read characters.
class ISource
{
public:
  /// @brief Exception thrown when a source stream error occurs (e.g., file not found).
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("ISource Error: ").append(message)) {}
  };

  virtual ~ISource() = default;

  /// @brief Return the character at the current stream position.
  [[nodiscard]] virtual Char current() const = 0;

  /// @brief Advance the stream position by one character.
  virtual void next() = 0;

  /// @brief Return `true` if there are more characters to read.
  [[nodiscard]] virtual bool more() const = 0;

  /// @brief Move the stream position back by @p length characters.
  virtual void backup(long length) = 0;

  /// @brief Return the current byte offset from the beginning of the stream.
  [[nodiscard]] virtual long position() const = 0;

  /// @brief Return the substring of the source between byte offsets @p start and @p end.
  virtual std::string getRange(long start, long end) = 0;

  /// @brief Reset the stream to the beginning.
  virtual void reset() = 0;

  /// @brief Return `true` if the current character is ASCII whitespace.
  [[nodiscard]] bool isWS() const { return std::iswspace(current()) != 0; }

  /// @brief Advance past all leading whitespace characters.
  void ignoreWS()
  {
    while (more() && isWS()) { next(); }
  }

  /// @brief Try to match @p target at the current position.
  /// @return `true` and advances the stream on success; `false` and leaves the stream unchanged on failure.
  bool match(const String &target)
  {
    long index = 0;
    while (more() && current() == target[index]) {
      next();
      if (++index == static_cast<long>(target.length())) { return true; }
    }
    backup(index);
    return false;
  }

  /// @brief Try to match the null-terminated string @p target at the current position.
  /// @return `true` and advances the stream on success; `false` and leaves the stream unchanged on failure.
  bool match(const char *target)
  {
    long index = 0;
    while (more() && current() == static_cast<Char>(target[index])) {
      next();
      if (++index == static_cast<long>(std::strlen(target))) { return true; }
    }
    backup(index);
    return false;
  }

  /// @brief Return the current `{line, column}` position within the source stream.
  [[nodiscard]] std::pair<long, long> getPosition() const { return std::make_pair(lineNo, columnNo); }

protected:
  long lineNo = 1;
  long columnNo = 1;
};
}// namespace XML_Lib