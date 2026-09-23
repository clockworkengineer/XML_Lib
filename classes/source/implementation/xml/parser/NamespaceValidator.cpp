#include "implementation/parser/NamespaceValidator.hpp"
#include "common/XML_Error.hpp"
#include "common/XML_QName.hpp"
#include <set>
#include <utility>

namespace XML_Lib {

void NamespaceValidator::validate(
  const Element &element,
  const ISource &source,
  const bool namespacesEnabled,
  const bool strictNamespaces)
{
  if (!namespacesEnabled) {
    return;
  }
  const std::string &elemName = element.name();
  validateQName(elemName, "element", source);
  if (const auto elemQName = splitQName(elemName); elemQName.hasPrefix()) {
    if (elemQName.prefix == "xmlns") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be used as the prefix of an element."));
    }
    if (elemQName.prefix != "xml" && !element.hasNameSpace(std::string(elemQName.prefix))) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Namespace used but not defined."));
    }
  }

  // Validate namespace declarations
  for (const auto &attr : element.getAttributes()) {
    const std::string &attrName = attr.getName();
    if (attrName == "xmlns") {
      const std::string &uri = attr.getParsed();
      if (uri == "http://www.w3.org/XML/1998/namespace") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The xml namespace must not be declared as the default namespace."));
      }
      if (uri == "http://www.w3.org/2000/xmlns/") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The xmlns namespace must not be declared as the default namespace."));
      }
    } else if (attrName == "xmlns:") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Empty prefix in namespace declaration."));
    } else if (attrName.starts_with("xmlns:")) {
      const std::string prefix = attrName.substr(6);
      if (prefix.empty() || prefix.find(':') != std::string::npos) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid prefix in namespace declaration."));
      }
      if (prefix == "xmlns") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be declared."));
      }
      const std::string &uri = attr.getParsed();
      if (strictNamespaces && uri.empty()) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Empty namespace URI in prefix declaration is not allowed in XML Namespaces 1.0."));
      }
      if (prefix == "xml") {
        if (uri != "http://www.w3.org/XML/1998/namespace") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xml' must only be bound to 'http://www.w3.org/XML/1998/namespace'."));
        }
      } else {
        if (uri == "http://www.w3.org/XML/1998/namespace") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The xml namespace can only be bound to prefix 'xml'."));
        }
        if (uri == "http://www.w3.org/2000/xmlns/") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The xmlns namespace must not be bound to any prefix."));
        }
      }
    }
  }

  // Validate regular attributes and attribute uniqueness under namespaces
  std::set<std::pair<std::string, std::string>> resolvedAttrs;
  for (const auto &attr : element.getAttributes()) {
    const std::string &attrName = attr.getName();
    if (attrName == "xmlns" || attrName.starts_with("xmlns:")) {
      continue;
    }
    std::string nsUri;
    std::string localPart = attrName;
    validateQName(attrName, "attribute", source);
    if (const auto attrQName = splitQName(attrName); attrQName.hasPrefix()) {
      localPart = std::string(attrQName.localName);
      const auto prefix = attrQName.prefix;
      if (prefix == "xmlns") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be used as the prefix of an attribute."));
      }
      if (prefix == "xml") {
        nsUri = "http://www.w3.org/XML/1998/namespace";
      } else if (element.hasNameSpace(std::string(prefix))) {
        nsUri = element.getNameSpace(std::string(prefix)).getParsed();
      } else {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Namespace used but not defined in attribute '" + attrName + "'."));
      }
    }
    if (!resolvedAttrs.insert({ nsUri, localPart }).second) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Attribute '" + attrName + "' defined more than once after namespace resolution."));
    }
  }
}

} // namespace XML_Lib
