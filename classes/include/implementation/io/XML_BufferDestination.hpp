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
  XML_LIB_DEFINE_ERROR("BufferDestination");
  // Constructors/Destructors
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() override = default;

  void add(const std::string &bytes) override
  {
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(buffer));
  }
  void add(const Char ch) override { add(toUtf8(ch)); }
  void add(const char * bytes) override {
    for (std::size_t index=0; index<strlen(bytes); index++) {
      buffer.push_back(bytes[index]);
    }
  }
  void add(const std::string_view &bytes) override {
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(buffer));
  }
  void clear() override { buffer.clear(); }

  [[nodiscard]] std::string toString() const { return buffer; }
  [[nodiscard]] std::size_t size() const { return buffer.size(); }

private:
  std::string buffer;
};
}// namespace XML_Lib