#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <codecvt>
#include <locale>
// =================
// LIBRARY NAMESPACE
// =================
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
  virtual void add(ISource::Char c) = 0;

protected:
  std::wstring_convert<std::codecvt_utf8_utf16<ISource::String::value_type>, ISource::String::value_type> m_UTF8;
};
}// namespace XML_Lib