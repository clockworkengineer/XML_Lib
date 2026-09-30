#pragma once

#if defined(__clang__) && defined(__cpp_concepts) && (__cpp_concepts < 202002L)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbuiltin-macro-redefined"
#undef __cpp_concepts
#define __cpp_concepts 202002L
#include <expected>
#pragma clang diagnostic pop
#else
#include <expected>
#endif

#include <string>

namespace XML_Lib {

/// @brief Error detail returned by non-throwing parseExpected and readExpected APIs (C++23).
struct XML_Error {
  std::string message;
  long line{ 0 };
  long column{ 0 };
};

} // namespace XML_Lib
