#pragma once

namespace XML_Lib {

class ISource;
struct Node;

/// @brief Strategy role interface for modular XML parse stages (SRP & OCP).
class IXMLParseStage
{
public:
  virtual ~IXMLParseStage() noexcept = default;

  /// @brief Execute parse stage on source stream.
  virtual void parseStage(ISource &source, Node &targetNode) = 0;
};

} // namespace XML_Lib
