#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

#include "IDestination.hpp"

namespace XML_Lib {

class BufferDestination : public IDestination
{
public:
  // Constructors/Destructors
  BufferDestination() = default;
  BufferDestination(const BufferDestination &other) = delete;
  BufferDestination &operator=(const BufferDestination &other) = delete;
  BufferDestination(BufferDestination &&other) = delete;
  BufferDestination &operator=(BufferDestination &&other) = delete;
  ~BufferDestination() = default;

  void add(const std::string &bytes) override
  {
    for (auto b : bytes) { buffer.push_back(b); }
  }
  void add(const XML_Lib::Char ch) override { add(converter.toUtf8(ch)); }
  [[nodiscard]] std::string getBuffer() const { return (buffer); }

private:
  std::string buffer;
};
}// namespace XML_Lib