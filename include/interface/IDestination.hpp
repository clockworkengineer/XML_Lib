#pragma once

#include <string>

#include "XML_Converter.hpp"

namespace XML_Lib {
// =======================================================================
// Interface for writing destination stream during XML/DTD stringification
// =======================================================================
class IDestination
{
public:
  // ==================
  // IDestination Error
  // ==================
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("IDestination Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IDestination() = default;
  // ========================
  // Add bytes to destination
  // ========================
  virtual void add(const std::string &bytes) = 0;
  virtual void add(XML_Lib::Char c) = 0;

};
}// namespace XML_Lib