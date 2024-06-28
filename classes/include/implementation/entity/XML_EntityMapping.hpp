#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

  struct XML_EntityMapping
  {
    // Mapping types
  protected:
    enum class Type { base = 0, internal, external, notation };

  public:
    XML_EntityMapping() = default;
    explicit XML_EntityMapping(const std::string &value) { setInternal(value); }
    // Entity reference get/set details
    [[nodiscard]] const std::string &getInternal() const { return internal; }
    [[nodiscard]] const std::string &getNotation() const { return notation; }
    [[nodiscard]] const XMLExternalReference &getExternal() const { return external; }
    void setInternal(const std::string &value)
    {
      mappingType = Type::internal;
      internal = value;
    }
    void setNotation(const std::string &value)
    {
      mappingType = Type::notation;
      notation = value;
    }
    void setExternal(const XMLExternalReference &value)
    {
      mappingType = Type::external;
      external = value;
    }
    [[nodiscard]] bool isInternal() const { return mappingType == Type::internal; }
    [[nodiscard]] bool isExternal() const { return mappingType == Type ::external; }
    [[nodiscard]] bool isNotation() const { return mappingType == Type::notation; }

  private:
    Type mappingType{ Type::base };
    std::string internal{};
    XMLExternalReference external{ "" };
    std::string notation{};
  };
}// namespace XML_Lib
