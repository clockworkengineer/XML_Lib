#pragma once

namespace XML_Lib {
// ===============================
// Construct XNode from raw values
// ===============================
template<typename T> XNode::XNode(T value)
{
  if constexpr (std::is_convertible_v<T, std::unique_ptr<Variant>>) { xmlVariant = std::move(value); }
}
}// namespace XML_Lib
