#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class XML_Impl
{
public:
  // Constructors/Destructors
  XML_Impl();
  XML_Impl(const XML_Impl &other) = delete;
  XML_Impl &operator=(const XML_Impl &other) = delete;
  XML_Impl(XML_Impl &&other) = delete;
  XML_Impl &operator=(XML_Impl &&other) = delete;
  ~XML_Impl();

  [[nodiscard]] XNode &dtd();
  [[nodiscard]] XNode &prolog();
  [[nodiscard]] XNode &root();
  [[nodiscard]] XNode &declaration();
  void parse(ISource &source);
  void stringify(IDestination &destination);
  void validate();
  [[nodiscard]] std::string version();

  [[nodiscard]] static std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, XML::Format format);
  [[nodiscard]] static XML::Format getFileFormat(const std::string &fileName);

private:
  // Root XNode
  XNode xmlRoot;
  // Entity mapper
  std::unique_ptr<IEntityMapper> entityMapper;
  // XML parser
  std::unique_ptr<IParser> parser;
  // XML stringifier
  std::unique_ptr<IStringify> stringifier;
};
}// namespace XML_Lib
