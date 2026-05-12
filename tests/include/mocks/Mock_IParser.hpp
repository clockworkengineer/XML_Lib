#pragma once

#include "interface/IParser.hpp"
#include "interface/ISource.hpp"
#include "XML.hpp"

#include <functional>
#include <string>

namespace XML_Lib {

/// @brief Configurable stub of IParser for unit-testing the XML class in isolation.
///
/// By default `parse()` returns a default-constructed Node and records that it
/// was called.  Supply `parseImpl` to override the behaviour for a specific test.
struct Mock_IParser : IParser
{
  /// Replace with a lambda to customise parse behaviour per test.
  std::function<Node(ISource &, const ParseOptions &)> parseImpl;

  bool parseCalled{ false };
  bool validateCalled{ false };
  bool canValidateResult{ false };

  Node parse(ISource &source, const ParseOptions &options) override
  {
    parseCalled = true;
    if (parseImpl) return parseImpl(source, options);
    return Node{};
  }

  bool canValidate() override { return canValidateResult; }

  void validate(Node &) override { validateCalled = true; }

  /// Reset recorded state between test sections.
  void reset()
  {
    parseCalled    = false;
    validateCalled = false;
    parseImpl      = nullptr;
  }
};

}// namespace XML_Lib
