#pragma once

#include "XML_Types.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Abstract interface for a writable character stream used during XML stringification.
///
/// Implementations (`BufferDestination`, `FileDestination`) accumulate serialised XML
/// into a string buffer or write it directly to a file.
class IDestination
{
public:
  /// @brief Exception thrown when a destination stream error occurs (e.g., file cannot be opened).
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IDestination Error: " ).append(message)) {}
  };

  virtual ~IDestination() = default;

  /// @brief Append a `std::string` to the destination.
  virtual void add(const std::string &bytes) = 0;

  /// @brief Append a single character to the destination.
  virtual void add(Char c) = 0;

  /// @brief Append a null-terminated C string to the destination.
  virtual void add(const char *bytes) = 0;

  /// @brief Append a `string_view` to the destination.
  virtual void add(const std::string_view &bytes) = 0;

  /// @brief Clear all previously written content from the destination.
  virtual void clear() = 0;
};
}// namespace XML_Lib