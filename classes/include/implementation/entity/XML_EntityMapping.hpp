#pragma once

namespace XML_Lib {

  struct XML_EntityMapping
  {
    // Mapping types
  protected:
    enum class Type { base = 0, internal, external, notation };

  public:
    XML_EntityMapping() = default;
    explicit XML_EntityMapping(const std::string_view &value) { setInternal(value); }
    // Entity reference get/set details
    [[nodiscard]] const std::string &getInternal() const { return internal; }
    [[nodiscard]] const std::string &getNotation() const { return notation; }
    [[nodiscard]] const XMLExternalReference &getExternal() const { return external; }
/// @brief
/// Implementation of setInternal.

    void setInternal(const std::string_view &value)
    {
      mappingType = Type::internal;
      internal = value;
    }
/// @brief
/// Implementation of setNotation.

    void setNotation(const std::string_view &value)
    {
      mappingType = Type::notation;
      notation = value;
    }
/// @brief
/// Implementation of setExternal.

    void setExternal(const XMLExternalReference &value)
    {
      mappingType = Type::external;
      external = value;
    }
    [[nodiscard]] bool isInternal() const { return mappingType == Type::internal; }
    [[nodiscard]] bool isExternal() const { return mappingType == Type ::external; }
    [[nodiscard]] bool isNotation() const { return mappingType == Type::notation; }
    [[nodiscard]] bool isFromExternalSubset() const { return fromExternalSubset; }
    void setFromExternalSubset(const bool val) { fromExternalSubset = val; }

  private:
    Type mappingType{ Type::base };
    std::string internal{};
    XMLExternalReference external{ "" };
    std::string notation{};
    bool fromExternalSubset{ false };
  };
}// namespace XML_Lib