#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XML_Parser.hpp"
#include "XML_Stringify.hpp"

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

  XNode &dtd();
  XNode &prolog();
  XNode &root();
  XNode &declaration();
  void parse(ISource &source);
  void stringify(IDestination &destination);
  void validate();
  std::string version();

  static const std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, XML::Format format);
  static XML::Format getFileFormat(const std::string &fileName);

private:
  // void stringify(const XNode &xNode, IDestination &destination);

  XNode xmlRoot;
  std::unique_ptr<IEntityMapper> entityMapper;
  std::unique_ptr<IParser> parser;
  std::unique_ptr<XML_Stringify> stringifier;
};
}// namespace XML_Lib
