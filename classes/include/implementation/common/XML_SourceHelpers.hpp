#pragma once

// XML_Sourceers.hpp
//
// Utility free-functions and member-function wrappers that operate on an
// ISource stream.  Separated from ISource so that the public interface stays
// as a pure-virtual contract while implementation code keeps the convenience
// helpers.
//
// Include this header (not ISource.hpp directly) in any .cpp that calls
// isWS / ignoreWS / match / getPosition on a source stream.

#include "interface/ISource.hpp"
#include <cstring>
#include <cwctype>
#include <utility>

namespace XML_Lib {

/// @brief Return `true` if the current source character is ASCII whitespace.
[[nodiscard]] inline bool isWS(const ISource &source) { return std::iswspace(source.current()) != 0; }

/// @brief Advance @p source past all leading whitespace characters.
inline void ignoreWS(ISource &source)
{
  while (source.more() && isWS(source)) { source.next(); }
}

/// @brief Try to match @p target at the current position of @p source.
/// Advances the stream and returns `true` on success; leaves the stream
/// unchanged and returns `false` on failure.
inline bool match(ISource &source, const String &target)
{
  long index = 0;
  while (source.more() && source.current() == target[index]) {
    source.next();
    if (++index == static_cast<long>(target.length())) { return true; }
  }
  source.backup(index);
  return false;
}

/// @brief Overload for null-terminated C strings.
inline bool match(ISource &source, const char *target)
{
  long index = 0;
  while (source.more() && source.current() == static_cast<Char>(target[index])) {
    source.next();
    if (++index == static_cast<long>(std::strlen(target))) { return true; }
  }
  source.backup(index);
  return false;
}

/// @brief Return the current `{line, column}` position within @p source.
[[nodiscard]] inline std::pair<long, long> getPosition(const ISource &source) { return source.getPosition(); }

}// namespace XML_Lib
