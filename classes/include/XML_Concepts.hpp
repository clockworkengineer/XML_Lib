#pragma once

#include "XML_Core.hpp"
#include <concepts>
#include <string_view>
#include <type_traits>

namespace XML_Lib {

/// @brief Concept constraining types that behave like an XML node.
template<typename T>
concept XMLNodeLike = requires(T t) {
  { t.getChildren() };
  { t.getVariant() };
  { t.isEmpty() } -> std::convertible_to<bool>;
};

/// @brief Concept constraining types that behave like a character input stream.
template<typename T>
concept XMLSourceLike = requires(T t) {
  { t.current() };
  { t.next() };
  { t.more() } -> std::convertible_to<bool>;
};

/// @brief Concept constraining types that behave like a character/string output destination.
template<typename T>
concept XMLDestinationLike = requires(T t, std::string_view sv, char ch) {
  { t.add(sv) };
  { t.add(ch) };
};

/// @brief Concept constraining types that can validate an XML node tree.
template<typename T>
concept XMLValidatorLike = requires(T t, const Node &node) {
  { t.validate(node) };
};

} // namespace XML_Lib
