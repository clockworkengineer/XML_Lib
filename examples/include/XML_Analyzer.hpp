
#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

class XML_Analyzer final : public XML_Lib::IAction
{
public:
  XML_Analyzer() = default;
  ~XML_Analyzer() override = default;
  // Add Node details to analysis
  void onNode([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalNodes++;
    sizeInBytes += sizeof(XML_Lib::Node);
  }
  void onCDATA([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalCDATA++;
    sizeInBytes += sizeof(XML_Lib::CDATA) + XML_Lib::NRef<XML_Lib::CDATA>(xNode).value().size();
  }
  void onComment([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalComment++;
    sizeInBytes += sizeof(XML_Lib::Comment)+ XML_Lib::NRef<XML_Lib::Comment>(xNode).value().size();
  }
  void onContent([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalContent++;
    sizeInBytes += sizeof(XML_Lib::Content)+ XML_Lib::NRef<XML_Lib::Content>(xNode).value().size();
  }
  void onDeclaration([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalDeclaration++;
    sizeInBytes += sizeof(XML_Lib::Declaration);
  }
  void onDTD([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalDTD++;
    sizeInBytes += sizeof(XML_Lib::DTD);
  }
  void onElement([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalElement++;
    sizeInBytes += sizeof(XML_Lib::Element);
  }
  void onEntityReference([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalEntityReference++;
    sizeInBytes += sizeof(XML_Lib::EntityReference);
  }
  void onPI([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalPI++;
    sizeInBytes += sizeof(XML_Lib::PI);
  }
  void onProlog([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalProlog++;
    sizeInBytes += sizeof(XML_Lib::Prolog);
  }
  void onRoot([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalRoot++;
    sizeInBytes += sizeof(XML_Lib::Root);
  }
  void onSelf([[maybe_unused]] const XML_Lib::Node &xNode) override
  {
    totalSelf++;
    sizeInBytes += sizeof(XML_Lib::Self);
  }
  // Output analysis details
  [[nodiscard]] std::string dump() const
  {
    std::stringstream os;
    os << "\n------------------XML Tree Stats------------------\n";
    os << "XML Tree contains " << totalNodes << " nodes.\n";
    os << "XML Tree size " << sizeInBytes << " in bytes.\n";
    os << "----------------------------------------------------";
    return (os.str());
  }
  static std::string dumpNodeSizes()
  {
    std::stringstream os;
    os << "\n--------------------XML_Lib::Node Sizes---------------------\n";
    os << "XML_Lib::XMLAttribute size " << sizeof(XML_Lib::XMLAttribute) << " in bytes.\n";
    os << "XML_Lib::XMLExternalReference size " << sizeof(XML_Lib::XMLExternalReference) << " in bytes.\n";
    os << "XML_Lib::XMLValue size " << sizeof(XML_Lib::XMLValue) << " in bytes.\n";
    os << "XML_Lib::Node size " << sizeof(XML_Lib::Node) << " in bytes.\n";
    os << "XML_Lib::XCDATA size " << sizeof(XML_Lib::CDATA) << " in bytes.\n";
    os << "XML_Lib::XComment Entry size " << sizeof(XML_Lib::Comment) << " in bytes.\n";
    os << "XML_Lib::XContent size " << sizeof(XML_Lib::Content) << " in bytes.\n";
    os << "XML_Lib::XDeclaration size " << sizeof(XML_Lib::Declaration) << " in bytes.\n";
    os << "XML_Lib::DTD size " << sizeof(XML_Lib::DTD) << " in bytes.\n";
    os << "XML_Lib::XElement size " << sizeof(XML_Lib::Element) << " in bytes.\n";
    os << "XML_Lib::XEntityReference size " << sizeof(XML_Lib::EntityReference) << " in bytes.\n";
    os << "XML_Lib::XPI size " << sizeof(XML_Lib::PI) << " in bytes.\n";
    os << "XML_Lib::XProlog size " << sizeof(XML_Lib::Prolog) << " in bytes.\n";
    os << "XML_Lib::XRoot size " << sizeof(XML_Lib::Root) << " in bytes.\n";
    os << "XML_Lib::XSelf size " << sizeof(XML_Lib::Self) << " in bytes.\n";
    return (os.str());
  }
  static std::string dumpNumericSizes()
  {
    std::stringstream os;
    os << "\n--------------------System Numeric Sizes---------------------\n";
    os << "Short size " << sizeof(short) << " in bytes.\n";
    os << "Int size " << sizeof(int) << " in bytes.\n";
    os << "Long Long size " << sizeof(long long) << " in bytes.\n";
    os << "Float size " << sizeof(float) << " in bytes.\n";
    os << "Double size " << sizeof(double) << " in bytes.\n";
    os << "Long Double size " << sizeof(long double) << " in bytes.\n";
    return (os.str());
  }

private:
  // XML analysis data
  // Node
  int64_t totalNodes{};
  size_t sizeInBytes{};
  // CDATA
  int64_t totalCDATA{};
  // Comment
  int64_t totalComment{};
  // Content
  int64_t totalContent{};
  // Declaration
  int64_t totalDeclaration{};
  // DTD
  int64_t totalDTD{};
  // Element
  int64_t totalElement{};
  // Entity Reference
  int64_t totalEntityReference{};
  // PI
  int64_t totalPI{};
  // Prolog
  int64_t totalProlog{};
  // Root
  int64_t totalRoot{};
  // Self
  int64_t totalSelf{};
};