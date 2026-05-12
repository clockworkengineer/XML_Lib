#pragma once

#include "interface/ISource.hpp"

#include <string>

namespace XML_Lib {

/// @brief Minimal in-memory stub of ISource for unit-testing individual parse helpers.
///
/// Wrap a UTF-8 string and expose it as a cursor-based character stream.
/// Only the methods required by the parser are implemented; auxiliary helpers
/// (backup, getRange) are provided with minimal correct behaviour.
struct Mock_ISource : ISource
{
  explicit Mock_ISource(std::string input) : data(std::move(input)) {}

  [[nodiscard]] Char current() const override
  {
    return pos < data.size() ? static_cast<Char>(static_cast<unsigned char>(data[pos])) : Char(0);
  }

  void next() override
  {
    if (pos < data.size()) {
      if (data[pos] == kLineFeed) { ++lineNo; columnNo = 1; }
      else { ++columnNo; }
      ++pos;
    }
  }

  [[nodiscard]] bool more() const override { return pos < data.size(); }

  void backup(long length) override
  {
    const auto delta = static_cast<std::size_t>(length < 0 ? 0 : length);
    pos = (delta > pos) ? 0 : pos - delta;
  }

  [[nodiscard]] long position() const override { return static_cast<long>(pos); }

  std::string getRange(long start, long end) override
  {
    const auto s = static_cast<std::size_t>(start < 0 ? 0 : start);
    const auto e = static_cast<std::size_t>(end   < 0 ? 0 : end);
    if (s >= data.size()) return {};
    return data.substr(s, e > s ? e - s : 0);
  }

  void reset() override { pos = 0; lineNo = 1; columnNo = 1; }

  // Test helpers
  std::string data;
  std::size_t pos{ 0 };
};

}// namespace XML_Lib
