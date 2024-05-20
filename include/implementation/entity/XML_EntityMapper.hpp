#pragma once

#include <string>
#include <map>
#include <filesystem>
#include <set>

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

struct XML_EntityMapper final : IEntityMapper
{
public:
  // Entity mapping data
  struct XML_EntityMapping
  {
    // Mapping types
  protected:
    enum class Type { base = 0, internal, external, notation };

  public:
    XML_EntityMapping() = default;
    explicit XML_EntityMapping(const std::string &value) { setInternal(value); }
    // Entity reference get/set details
    const std::string &getInternal() const { return internal; }
    const std::string &getNotation() const { return notation; }
    const XMLExternalReference &getExternal() const { return external; }
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
  // EntityMapper Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("EntityMapper Error: " + message) {}
  };
  // Constructors/Destructors
  XML_EntityMapper();
  XML_EntityMapper(const XML_EntityMapper &other) = delete;
  XML_EntityMapper &operator=(const XML_EntityMapper &other) = delete;
  XML_EntityMapper(XML_EntityMapper &&other) = delete;
  XML_EntityMapper &operator=(XML_EntityMapper &&other) = delete;
  ~XML_EntityMapper() override;
  // Is entity reference mapping entry present ?
  [[nodiscard]] bool isPresent(const std::string &entityName) const override;
  // Determine entity type
  [[nodiscard]] bool isInternal(const std::string &entityName) override;
  [[nodiscard]] bool isExternal(const std::string &entityName) override;
  [[nodiscard]] bool isNotation(const std::string &entityName) override;
  // Entity reference get/set details
  [[nodiscard]] const std::string &getInternal(const std::string &entityName) override;
  [[nodiscard]] const std::string &getNotation(const std::string &entityName) override;
  [[nodiscard]] const XMLExternalReference &getExternal(const std::string &entityName) override;
  void setInternal(const std::string &entityName, const std::string &internal) override;
  void setNotation(const std::string &entityName, const std::string &notation) override;
  void setExternal(const std::string &entityName, const XMLExternalReference &external) override;
  //  Get mapping for an entity reference
  [[nodiscard]] XMLValue map(const XMLValue &entityReference) override;
  // Translate any entity reference in a string
  [[nodiscard]] std::string translate(const std::string &toTranslate, char type = '%') const override;
  // Check for a recursive entity reference mapping
  void checkForRecursion() override;
  // Reset entity mapper to default state
  void reset() override;

private:
  //  Check for any recursion in a entity reference
  void
    recurseOverEntityReference(const std::string &entityName, Char type, std::set<std::string> &currentEntities);
  // Get contents of file that is pointed to by an entity reference
  [[nodiscard]] static std::string getFileMappingContents(const std::string &fileName);
  // Get entity reference mapping entry
  [[nodiscard]] XML_EntityMapping &getEntityMapping(const std::string &entityName);
  // Reset entity map
  void resetToDefault();

  std::map<std::string, XML_EntityMapping> entityMappings;
};
}// namespace XML_Lib
