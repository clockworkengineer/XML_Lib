#pragma once

namespace XML_Lib {

struct XML_EntityMapper final : IEntityMapper
{
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
  // Is entity reference mapping entry present?
  [[nodiscard]] bool isPresent(const std::string_view &entityName) const override;
  // Determine entity type
  [[nodiscard]] bool isInternal(const std::string_view &entityName) override;
  [[nodiscard]] bool isExternal(const std::string_view &entityName) override;
  [[nodiscard]] bool isNotation(const std::string_view &entityName) override;
  // Entity reference get/set details
  [[nodiscard]] const std::string &getInternal(const std::string_view &entityName) override;
  [[nodiscard]] const std::string &getNotation(const std::string_view &entityName) override;
  [[nodiscard]] const XMLExternalReference &getExternal(const std::string_view &entityName) override;
  void setInternal(const std::string_view &entityName, const std::string_view &internal) override;
  void setNotation(const std::string_view &entityName, const std::string_view &notation) override;
  void setExternal(const std::string_view &entityName, const XMLExternalReference &external) override;
  //  Get mapping for an entity reference
  [[nodiscard]] XMLValue map(const XMLValue &entityReference) override;
  // Translate any entity reference in a string
  [[nodiscard]] std::string translate(const std::string_view &toTranslate, char type = '%') const override;
  // Check for a recursive entity reference mapping
  void checkForRecursion() override;
  // Reset entity mapper to default state
  void reset() override;

private:
  //  Check for any recursion in an entity reference
  void
    recurseOverEntityReference(const std::string &entityName, Char type, std::set<std::string> &currentEntities);
  // Get the contents of the file that is pointed to by an entity reference
  [[nodiscard]] static std::string getFileMappingContents(const std::string &fileName);
  // Get entity reference mapping entry
  [[nodiscard]] XML_EntityMapping &getEntityMapping(const std::string_view &entityName);
  // Reset entity map
  void resetToDefault();

  std::map<std::string, XML_EntityMapping> entityMappings;
};
}// namespace XML_Lib
