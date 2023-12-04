#pragma once

#include <memory>
#include <string>
#include <stdexcept>

namespace XML_Lib {

// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class XML_Impl;
class ISource;
class IDestination;
struct XNode;

class XML
{
public:
  // XML error types
  struct Error : public std::runtime_error
  {
    Error(std::string const &message) : std::runtime_error("XML Error: " + message) {}
  };

  struct SyntaxError : public std::runtime_error
  {
    SyntaxError(const std::string &message) : std::runtime_error("XML Syntax Error: " + message) {}
    explicit SyntaxError(const std::pair<long, long> &position, const std::string &message = "")
      : std::runtime_error("XML Syntax Error [Line: " + std::to_string(position.first)
                           + " Column: " + std::to_string(position.second) + "] " + message)
    {}
  };

  struct ValidationError : public std::runtime_error
  {
    explicit ValidationError(long lineNumber, const std::string &message = "")
      : std::runtime_error("XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + message)
    {}
  };

  // Constructors/Destructors
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
