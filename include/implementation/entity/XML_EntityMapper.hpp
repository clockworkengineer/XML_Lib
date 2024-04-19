#pragma once

#include <string>
#include <map>
#include <filesystem>
#include <set>

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class XML_EntityMapper : public IEntityMapper
{
public:
  // Entity mapping data
  struct XMLEntityMapping
  {
    explicit XMLEntityMapping(const std::string &internal) : internal(internal) {}
    std::string internal{};
    XMLExternalReference external{ "" };
    std::string notation{};
  };

  // Constructors/Destructors
  XML_EntityMapper();
  XML_EntityMapper(const XML_EntityMapper &other) = delete;
  XML_EntityMapper &operator=(const XML_EntityMapper &other) = delete;
  XML_EntityMapper(XML_EntityMapper &&other) = delete;
  XML_EntityMapper &operator=(XML_EntityMapper &&other) = delete;
  ~XML_EntityMapper();

  // Is entity reference mapping entry present ?
  [[nodiscard]] bool isPresent(const std::string &entityName) const override;
  // Entity reference get/set details
  const std::string &getInternal(const std::string &entityName) override;
  const std::string &getNotation(const std::string &entityName) override;
  const XMLExternalReference &getExternal(const std::string &entityName) override;
  void setInternal(const std::string &entityName, const std::string &internal) override;
  void setNotation(const std::string &entityName, const std::string &notation) override;
  void setExternal(const std::string &entityName, const XMLExternalReference &external) override;
  //  Get mapping for an entity reference
  XMLValue map(const XMLValue &entityReference) override;
  // Translate any entity reference in a string
  [[nodiscard]] std::string translate(const std::string &toTranslate, char type = '%') const override;
  // Check for a recursive entity reference mapping
  void checkForRecursion() override;

  void resetToDefault() override;

private:
  //  Check for any recursion in a entity reference
  void
    recurseOverEntityReference(const std::string &entityReference, Char type, std::set<std::string> &currentEntities);
  // Get contents of file that is pointed to by an entity reference
  std::string getFileMappingContents(const std::string &fileName);
  // Get entity reference mapping entry
  XMLEntityMapping &getEntityMapping(const std::string &entityName);

  std::map<std::string, XMLEntityMapping> entityMappings;
};
}// namespace XML_Lib
