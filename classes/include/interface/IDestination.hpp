#pragma once

#include "XML_Types.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Segregated interface for character-by-character output.
class ICharWriter
{
public:
  virtual ~ICharWriter() noexcept = default;

  /// @brief Append a single character to the destination.
  virtual void add(Char c) = 0;
};

/// @brief Segregated interface for string/block output.
class IStringWriter
{
public:
  virtual ~IStringWriter() noexcept = default;

  /// @brief Append a `std::string` to the destination.
  virtual void add(const std::string &bytes) = 0;

  /// @brief Append a null-terminated C string to the destination.
  virtual void add(const char *bytes) = 0;

  /// @brief Append a `string_view` to the destination.
  virtual void add(const std::string_view &bytes) = 0;
};

/// @brief Segregated interface for resettable destinations.
class IResettableDestination
{
public:
  virtual ~IResettableDestination() noexcept = default;

  /// @brief Clear all previously written content from the destination.
  virtual void clear() = 0;
};

/// @brief Abstract composite interface for a writable character stream used during XML stringification.
///
/// Implementations (`BufferDestination`, `FileDestination`) accumulate serialised XML
/// into a string buffer or write it directly to a file.
class IDestination : public ICharWriter, public IStringWriter, public IResettableDestination
{
public:
  using ICharWriter::add;
  using IStringWriter::add;

  /// @brief Exception thrown when a destination stream error occurs (e.g., file cannot be opened).
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IDestination Error: " ).append(message)) {}
  };

  ~IDestination() noexcept override = default;
};

} // namespace XML_Lib