#pragma once

#include "implementation/io/XML_BufferSource.hpp"
#include "implementation/io/XML_FileSource.hpp"
#include "interface/ISource.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Factory class to create ISource instances without coupling high-level modules to concrete source types.
class SourceFactory
{
public:
  /// @brief Create a BufferSource instance as unique_ptr to ISource.
  [[nodiscard]] static std::unique_ptr<ISource> createBufferSource(
    const std::string_view &buffer,
    std::size_t maxBytes = XML_LIB_MAX_XML_SIZE,
    std::string_view systemId = "<buffer>")
  {
    return std::make_unique<BufferSource>(buffer, maxBytes, systemId);
  }

  /// @brief Create a FileSource instance as unique_ptr to ISource.
  [[nodiscard]] static std::unique_ptr<ISource> createFileSource(
    const std::string_view &fileName,
    std::size_t maxBytes = XML_LIB_MAX_XML_SIZE)
  {
    return std::make_unique<FileSource>(fileName, maxBytes);
  }
};

}// namespace XML_Lib
