#pragma once

#include <string>
// ===================
// Character converter
// ===================
#include "Converter.hpp"

namespace XML_Lib {
// =======================================================================
// Interface for writing destination stream during XML/DTD stringification
// =======================================================================
class IDestination : protected Converter
{
public:
  // ==================
  // IDestination Error
  // ==================
  struct Error : public std::runtime_error
  {
    Error(std::string const &message) : std::runtime_error("IDestination Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  IDestination() = default;
  IDestination(const IDestination &other) = delete;
  IDestination &operator=(const IDestination &other) = delete;
  IDestination(IDestination &&other) = delete;
  IDestination &operator=(IDestination &&other) = delete;
  virtual ~IDestination() = default;
  // ========================
  // Add bytes to destination
  // ========================
  virtual void add(const std::string &bytes) = 0;
  virtual void add(XML_Lib::Char c) = 0;
};
}// namespace XML_Lib