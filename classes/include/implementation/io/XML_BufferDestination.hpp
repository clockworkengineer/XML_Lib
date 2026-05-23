#pragma once

#include "common/XML_Error.hpp"
#include "XML_Converter.hpp"
#include "IDestination.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

class BufferDestination final : public IDestination
{
public:
  // BufferDestination Error
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("BufferDestination");
#endif
  // Constructors/Destructors
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() override = default;

  void add(const std::string &bytes) override
  {
    buffer.append(bytes);
  }
  void add(const Char ch) override { add(toUtf8(ch)); }
/// @brief
/// Implementation of add.

  void add(const char * bytes, std::size_t length)
  {
    if (!bytes && length > 0) { XML_LIB_THROW(Error("Null pointer passed to BufferDestination::add().")); }
    if (length == 0) { return; }
    buffer.append(bytes, length);
  }
  void add(const char * bytes) override {
    if (!bytes) { XML_LIB_THROW(Error("Null pointer passed to BufferDestination::add().")); }
    const std::size_t length = std::strlen(bytes);
    add(bytes, length);
  }
  void add(const std::string_view &bytes) override {
    if (bytes.empty()) { return; }
    buffer.append(bytes.data(), bytes.size());
  }
  void clear() override { buffer.clear(); }

  [[nodiscard]] std::string toString() const { return buffer; }
  [[nodiscard]] std::size_t size() const { return buffer.size(); }

private:
  std::string buffer;
};
}// namespace XML_Lib