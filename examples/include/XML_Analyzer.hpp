
#pragma once

#include <set>
#include <sstream>

#include "XML.hpp"
#include "XML_Core.hpp"

class XML_Analyzer final : public XML_Lib::IAction
{
public:
  XML_Analyzer() = default;
  ~XML_Analyzer() override = default;
  // Add XNode details to analysis
  void onXNode([[maybe_unused]] const XML_Lib::XNode &xNode) override { totalNodes++; }
  // Output analysis details
  [[nodiscard]] std::string dump() const
  {
    std::stringstream os;
    os << "\n------------------XML Tree Stats------------------\n";
    os << "XML Tree contains " << totalNodes << " nodes.\n";
    os << "XML Tree size " << sizeInBytes << " in bytes.\n";
    // os << "------------------XML XML_Lib::Object Stats------------------\n";
    // os << "XML Tree contains " << totalObjects << " objectEntries.\n";
    // os << "XML Tree max object size " << maxObjectSize << ".\n";
    // os << "XML Tree total " << totalKeys << " keys.\n";
    // os << "XML Tree contains " << uniqueKeys.size() << " unique keys.\n";
    // os << "XML Tree max key size " << maxKeySize << " in bytes.\n";
    // os << "------------------XML XML_Lib::Array Stats------------------\n";
    // os << "XML Tree contains " << totalArrays << " arrays.\n";
    // os << "XML Tree max array size " << maxArraySize << ".\n";
    // os << "------------------XML XML_Lib::String Stats------------------\n";
    // os << "XML Tree total " << totalStrings << " strings.\n";
    // os << "XML Tree contains " << uniqueStrings.size() << " unique strings.\n";
    // os << "XML Tree max string size " << maxStringSize << " in bytes.\n";
    // os << "------------------XML XML_Lib::Number Stats------------------\n";
    // os << "XML Tree contains " << totalNumbers << " numbers.\n";
    // os << "XML Tree contains " << totalInteger << " integers.\n";
    // os << "XML Tree contains " << totalLong << " longs.\n";
    // os << "XML Tree contains " << totalLongLong << " long longs.\n";
    // os << "XML Tree contains " << totalFloat << " floats.\n";
    // os << "XML Tree contains " << totalDouble << " doubles.\n";
    // os << "XML Tree contains " << totalLongDouble << " long doubles.\n";
    // os << "------------------XML XML_Lib::Boolean Stats------------------\n";
    // os << "XML Tree contains " << totalBoolean << " booleans.\n";
    // os << "------------------XML XML_Lib::Null Stats------------------\n";
    // os << "XML Tree contains " << totalNull << " nulls.\n";
    os << "----------------------------------------------------";
    return (os.str());
  }
  static std::string dumpXNodeSizes()
  {
    std::stringstream os;
    os << "\n--------------------XML_Lib::XNode Sizes---------------------\n";
    os << "XML_Lib::XNode size " << sizeof(XML_Lib::XNode) << " in bytes.\n";
    // os << "XML_Lib::Object size " << sizeof(XML_Lib::Object) << " in bytes.\n";
    // os << "XML_Lib::Object Entry size " << sizeof(XML_Lib::Object::Entry) << " in bytes.\n";
    // os << "XML_Lib::Array size " << sizeof(XML_Lib::Array) << " in bytes.\n";
    // os << "XML_Lib::Number::Values size " << sizeof(XML_Lib::Number::Values) << " in bytes.\n";
    // os << "XML_Lib::Number size " << sizeof(XML_Lib::Number) << " in bytes.\n";
    // os << "XML_Lib::String size " << sizeof(XML_Lib::String) << " in bytes.\n";
    // os << "XML_Lib::Boolean size " << sizeof(XML_Lib::Boolean) << " in bytes.\n";
    // os << "XML_Lib::Null size " << sizeof(XML_Lib::Null) << " in bytes.\n";
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
  // XML_Lib::Object
  // int64_t totalObjects{};
  // size_t maxObjectSize{};
  // int64_t totalKeys{};
  // size_t maxKeySize{};
  // std::set<std::string> uniqueKeys{};
  // // XML_Lib::Array
  // size_t maxArraySize{};
  // int64_t totalArrays{};
  // // XML_Lib::String
  // int64_t totalStrings{};
  // std::set<std::string> uniqueStrings{};
  // size_t maxStringSize{};
  // // XML_Lib::Number
  // int64_t totalNumbers{};
  // int64_t totalInteger{};
  // int64_t totalLong{};
  // int64_t totalLongLong{};
  // int64_t totalFloat{};
  // int64_t totalDouble{};
  // int64_t totalLongDouble{};
  // // XML_Lib::Boolean
  // int64_t totalBoolean{};
  // // XML_Lib::Null
  // int64_t totalNull{};
};