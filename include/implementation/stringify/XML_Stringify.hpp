#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XML_Parser.hpp"

namespace XML_Lib {

class XML_Stringify : public IStringify
{
public:
  // Constructors/Destructors
  XML_Stringify() = default;
  XML_Stringify(const XML_Stringify &other) = delete;
  XML_Stringify &operator=(const XML_Stringify &other) = delete;
  XML_Stringify(XML_Stringify &&other) = delete;
  XML_Stringify &operator=(XML_Stringify &&other) = delete;
  ~XML_Stringify() = default;

  virtual void stringify(const XNode &xNode, IDestination &destination) const ;

private:

  // void stringify(const XNode &xNode, IDestination &destination);

//   XNode xmlRoot;
//   std::unique_ptr<IEntityMapper> entityMapper;
//   std::unique_ptr<IParser> parser;
};
}// namespace XML_Lib
