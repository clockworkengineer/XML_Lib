#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

#include "IDestination.hpp"

namespace XML_Lib {

class BufferDestination : public IDestination
{
public:
  // BufferDestination Error
  struct Error : public std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("BufferDestination Error: " + message) {}
  };
  // Constructors/Destructors
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() = default;

  void add(const std::string &bytes) override
  {
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(buffer));
  }
  void add(const Char ch) override { add(toUtf8(ch)); }
  [[nodiscard]] std::string getBuffer() const { return buffer; }

private:
  std::string buffer;
};
}// namespace XML_Lib