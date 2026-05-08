#pragma once

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
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("BufferDestination Error: ").append(message)) {}
  };
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