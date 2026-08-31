#pragma once

#include "XML_Types.hpp"
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

/// @brief Segregated interface for character stream navigation.
class ICharStream
{
public:
  virtual ~ICharStream() noexcept = default;

  /// @brief Return the character at the current stream position.
  [[nodiscard]] virtual Char current() const = 0;

  /// @brief Advance the stream position by one character.
  virtual void next() = 0;

  /// @brief Return `true` if there are more characters to read.
  [[nodiscard]] virtual bool more() const = 0;

  /// @brief Move the stream position back by @p length characters.
  virtual void backup(long length) = 0;
};

/// @brief Segregated interface for position and location tracking.
class ILocationTracker
{
public:
  virtual ~ILocationTracker() noexcept = default;

  /// @brief Return the current byte/char offset from the beginning of the stream.
  [[nodiscard]] virtual long position() const = 0;

  /// @brief Return the current `{line, column}` position within the source stream.
  [[nodiscard]] virtual std::pair<long, long> getPosition() const = 0;

  /// @brief Return system identifier / URI / name associated with the source.
  [[nodiscard]] virtual std::string getSystemId() const = 0;
};

/// @brief Segregated interface for range/substring extraction.
class IRangeReader
{
public:
  virtual ~IRangeReader() noexcept = default;

  /// @brief Return the substring of the source between character/byte offsets @p start and @p end.
  virtual std::string getRange(long start, long end) = 0;
};

/// @brief Segregated interface for resettable streams.
class IResettableStream
{
public:
  virtual ~IResettableStream() noexcept = default;

  /// @brief Reset the stream to the beginning.
  virtual void reset() = 0;
};

/// @brief Pure abstract composite interface for a readable character source used during XML/DTD parsing.
class ISource : public ICharStream, public ILocationTracker, public IRangeReader, public IResettableStream
{
public:
  /// @brief Exception thrown when a source stream error occurs (e.g., file not found).
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("ISource Error: ").append(message)) {}
  };

  ~ISource() noexcept override = default;
};

}// namespace XML_Lib