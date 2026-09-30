#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "implementation/stringify/INodeSerializer.hpp"
#include <unordered_map>
#include <memory>

namespace XML_Lib {

class PrologSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    unsigned long indent,
    const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    for (auto &child : xNode.getChildren()) { recurse(child, destination, indent); }
  }
};

class DeclarationSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    auto &xNodeDeclaration = NRef<Declaration>(xNode);
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
};

class ElementSerializer final : public INodeSerializer
{
public:
  explicit ElementSerializer(const StringifyOptions &options) : options(options) {}

  void serialize(
    const Node &xNode,
    IDestination &destination,
    unsigned long indent,
    const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &xElement = NRef<Element>(xNode);
    destination.add("<" + xElement.name());
    for (auto &attribute : xElement.getAttributes()) {
      if (options.prettyPrint && options.attributeNewlineWrapping) {
        destination.add("\n");
        appendIndent(destination, indent + 1);
      } else {
        destination.add(" ");
      }
      destination.add(attribute.getName() + "=" + attribute.getQuote() + attribute.getUnparsed() + attribute.getQuote());
    }
    if (!isA<Self>(xNode)) {
      destination.add(">");
      for (auto &child : xNode.getChildren()) { recurse(child, destination, indent + 1); }
      destination.add("</" + xElement.name() + ">");
    } else {
      if (options.selfClosingSpacing) {
        destination.add(" />");
      } else {
        destination.add("/>");
      }
    }
  }

private:
  void appendIndent(IDestination &dest, unsigned long level) const
  {
    const char indentChar = options.useTabs ? '\t' : ' ';
    const int count = options.useTabs ? static_cast<int>(level) : static_cast<int>(level * options.indentSpaces);
    for (int i = 0; i < count; ++i) { dest.add(indentChar); }
  }

  const StringifyOptions &options;
};

class CommentSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &xNodeComment = NRef<Comment>(xNode);
    destination.add("<!--" + xNodeComment.value() + "-->");
  }
};

class ContentSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &xNodeContent = NRef<Content>(xNode);
    destination.add(xNodeContent.value());
  }
};

class EntityReferenceSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &xNodeEntity = NRef<EntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
};

class PISerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const PI &xNodePI = NRef<PI>(xNode);
    destination.add("<?" + xNodePI.name() + (xNodePI.parameters().empty() ? "" : " " + xNodePI.parameters()) + "?>");
  }
};

class CDATASerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &xNodeCDATA = NRef<CDATA>(xNode);
    destination.add("<![CDATA[" + std::string(xNodeCDATA.value()) + "]]>");
  }
};

class DTDSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    destination.add(NRef<DTD>(xNode).unparsed());
  }
};

/// @brief Strategy-based XML stringifier enforcing Open/Closed Principle (OCP).
class Default_Stringify final : public IIndentedStringify
{
public:
  Default_Stringify()
  {
    initSerializers();
  }

  void registerSerializer(Variant::Type nodeType, std::unique_ptr<INodeSerializer> serializer)
  {
    serializers[nodeType] = std::move(serializer);
  }

  void stringify(const Node &xNode, IDestination &destination, const unsigned long indent) const override
  {
    stringifyNodes(xNode, destination, indent);
  }

  [[nodiscard]] long getIndent() const override { return options.indentSpaces; }
  void setIndent(long indent) override
  {
    options.indentSpaces = static_cast<int>(indent);
    options.prettyPrint = (indent > 0);
    initSerializers();
  }

  void setOptions(const StringifyOptions &opts) override
  {
    options = opts;
    initSerializers();
  }

  [[nodiscard]] const StringifyOptions &getOptions() const override
  {
    return options;
  }

private:
  void initSerializers()
  {
    serializers.clear();
    registerSerializer(Variant::Type::prolog, std::make_unique<PrologSerializer>());
    registerSerializer(Variant::Type::declaration, std::make_unique<DeclarationSerializer>());
    registerSerializer(Variant::Type::root, std::make_unique<ElementSerializer>(options));
    registerSerializer(Variant::Type::element, std::make_unique<ElementSerializer>(options));
    registerSerializer(Variant::Type::self, std::make_unique<ElementSerializer>(options));
    registerSerializer(Variant::Type::comment, std::make_unique<CommentSerializer>());
    registerSerializer(Variant::Type::content, std::make_unique<ContentSerializer>());
    registerSerializer(Variant::Type::entity, std::make_unique<EntityReferenceSerializer>());
    registerSerializer(Variant::Type::pi, std::make_unique<PISerializer>());
    registerSerializer(Variant::Type::cdata, std::make_unique<CDATASerializer>());
    registerSerializer(Variant::Type::dtd, std::make_unique<DTDSerializer>());
  }

  void stringifyNodes(const Node &xNode, IDestination &destination, const unsigned long indent) const
  {
    const auto it = serializers.find(xNode.getVariant().getNodeType());
    if (it != serializers.end()) {
      it->second->serialize(xNode, destination, indent, [this](const Node &n, IDestination &d, unsigned long i) {
        stringifyNodes(n, d, i);
      });
    } else {
      XML_LIB_THROW(Error("Invalid Node encountered during stringify."));
    }
  }

  StringifyOptions options{};
  std::unordered_map<Variant::Type, std::unique_ptr<INodeSerializer>> serializers;
};

} // namespace XML_Lib