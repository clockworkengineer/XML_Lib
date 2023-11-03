#pragma once

#include <memory>
#include <string>

namespace XML_Lib {

#include "XML_Forward.hpp"

class XML
{
public:
  XML();
  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;
  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;
  ~XML();

  // Get reference to DTD XNode
  XNode &dtd();
  // Get reference to proglog XNode
  XNode &prolog();
  // Get reference to declaration XNode
  XNode &declaration();
  // Get reference to root element XNode
  XNode &root();
  // Validate XML
  void validate();
  // Return version string
  std::string version();
  // Parse XML source
  void parse(ISource &source);
  void parse(ISource &&source);
  // Stringify XML object to destination text
  void stringify(IDestination &destination);
  void stringify(IDestination &&destination);

private:
  const std::unique_ptr<XML_Impl> implementation;
};
}// namespace XML_Lib
