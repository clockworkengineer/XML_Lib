#pragma once
//
// C++ STL
//
#include <stdexcept>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
//
// Source interface
//
#include "ISource.hpp"
//
// XML types/nodes
//
#include "XML_Types.hpp"
#include "XML_Nodes.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib::Core
{
    //
    // XML validation
    //
    bool validChar(ISource::Char ch);
    bool validNameStartChar(ISource::Char ch);
    bool validNameChar(ISource::Char ch);
    bool validReservedName(const ISource::String &name);
    bool validName(ISource::String name);
    bool validAttributeValue(const XMLValue &value);
    //
    // XML parsing
    //
    XMLValue parseEntityReference(ISource &source);
    std::string parseName(ISource &source);
    XMLValue parseCharacterReference(ISource &source);
    XMLValue parseCharacter(ISource &source);
    XMLValue parseValue(ISource &source);
    XMLValue parseValue(ISource &source, IXMLEntityMapper &entityMapper);
    std::string parseTagBody(ISource &source);
    //
    // XML utility
    //
    std::vector<std::string> splitString(const std::string &stringToSplit, char delimeter);
    std::string trimmString(const std::string &stringToTrimm);
    std::string toUpperString(const std::string &stringToUpper);
} // namespace XMLLib::Core
