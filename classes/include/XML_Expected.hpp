#pragma once

#include <string>

namespace XML_Lib {

/// @brief Error detail returned by non-throwing parseExpected and readExpected APIs (C++23).
struct XML_Error {
  std::string message;
  long line{ 0 };
  long column{ 0 };
};

} // namespace XML_Lib
