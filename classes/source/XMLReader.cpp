//
// Class: XMLReader
//
// Description: Forward-only streaming pull-parser cursor.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XMLReader.hpp"
#include "common/XML_Error.hpp"
#include "common/XML_ParseHelpers.hpp"
#include "common/XML_SourceHelpers.hpp"
#include "converter/XML_Converter.hpp"

namespace XML_Lib {

XMLReader::XMLReader(ISource &source) : sourceStream(&source) {}

XMLReader::XMLReader(std::string_view xmlString)
  : ownedBufferSource(std::make_unique<BufferSource>(xmlString))
{
  sourceStream = ownedBufferSource.get();
}

XMLReader XMLReader::fromFile(const std::filesystem::path &filePath)
{
  XMLReader reader(std::string_view{});
  reader.ownedFileSource = std::make_unique<FileSource>(filePath.string());
  reader.sourceStream = reader.ownedFileSource.get();
  reader.ownedBufferSource.reset();
  return reader;
}

XMLReader::XMLReader(XMLReader &&other) noexcept
  : ownedBufferSource(std::move(other.ownedBufferSource))
  , ownedFileSource(std::move(other.ownedFileSource))
  , sourceStream(other.sourceStream == other.ownedBufferSource.get()
                   ? nullptr
                   : (other.sourceStream == other.ownedFileSource.get() ? nullptr : other.sourceStream))
  , type(other.type)
  , currentName(std::move(other.currentName))
  , currentValue(std::move(other.currentValue))
  , currentAttributes(std::move(other.currentAttributes))
  , openElements(std::move(other.openElements))
  , currentDepth(other.currentDepth)
  , isSelfClosing(other.isSelfClosing)
  , pendingEndElement(other.pendingEndElement)
  , atEnd(other.atEnd)
  , skipWhitespace(other.skipWhitespace)
{
  if (!sourceStream) {
    if (ownedBufferSource) {
      sourceStream = ownedBufferSource.get();
    } else if (ownedFileSource) {
      sourceStream = ownedFileSource.get();
    }
  }
}

XMLReader &XMLReader::operator=(XMLReader &&other) noexcept
{
  if (this != &other) {
    ownedBufferSource = std::move(other.ownedBufferSource);
    ownedFileSource = std::move(other.ownedFileSource);
    if (other.sourceStream == other.ownedBufferSource.get()) {
      sourceStream = ownedBufferSource.get();
    } else if (other.sourceStream == other.ownedFileSource.get()) {
      sourceStream = ownedFileSource.get();
    } else {
      sourceStream = other.sourceStream;
    }
    type = other.type;
    currentName = std::move(other.currentName);
    currentValue = std::move(other.currentValue);
    currentAttributes = std::move(other.currentAttributes);
    openElements = std::move(other.openElements);
    currentDepth = other.currentDepth;
    isSelfClosing = other.isSelfClosing;
    pendingEndElement = other.pendingEndElement;
    atEnd = other.atEnd;
    skipWhitespace = other.skipWhitespace;
  }
  return *this;
}

XMLReader::~XMLReader() = default;

bool XMLReader::read()
{
  if (!sourceStream || atEnd) {
    type = NodeType::EndDocument;
    return false;
  }

  // Synthesize an ElementEnd event right after an empty/self-closing element
  if (pendingEndElement) {
    pendingEndElement = false;
    type = NodeType::ElementEnd;
    isSelfClosing = true;
    currentAttributes.clear();
    currentValue.clear();
    currentDepth = static_cast<int>(openElements.size());
    return true;
  }

  while (sourceStream->more()) {
    currentName.clear();
    currentValue.clear();
    currentAttributes.clear();
    isSelfClosing = false;

    // Check for XML markup
    if (sourceStream->current() == '<') {
      if (match(*sourceStream, "<!--")) {
        type = NodeType::Comment;
        currentValue = parseCommentBody(*sourceStream);
        currentDepth = static_cast<int>(openElements.size());
        return true;
      }
      if (match(*sourceStream, "<![CDATA[")) {
        type = NodeType::CDATA;
        String cdata;
        cdata.reserve(64);
        while (sourceStream->more()) {
          if (match(*sourceStream, "]]>")) { break; }
          cdata += sourceStream->current();
          sourceStream->next();
        }
        currentValue = toUtf8(cdata);
        currentDepth = static_cast<int>(openElements.size());
        return true;
      }
      if (match(*sourceStream, "<!DOCTYPE")) {
        type = NodeType::DTD;
        int bracketDepth = 0;
        char quote = 0;
        std::string dtd = "<!DOCTYPE";
        while (sourceStream->more()) {
          const Char ch = sourceStream->current();
          sourceStream->next();
          dtd += toUtf8(ch);
          if (quote != 0) {
            if (ch == static_cast<Char>(quote)) { quote = 0; }
          } else if (ch == '"' || ch == '\'') {
            quote = static_cast<char>(ch);
          } else if (ch == '[') {
            ++bracketDepth;
          } else if (ch == ']') {
            if (bracketDepth > 0) { --bracketDepth; }
          } else if (ch == '>' && bracketDepth == 0) {
            break;
          }
        }
        currentValue = std::move(dtd);
        currentDepth = static_cast<int>(openElements.size());
        return true;
      }
      if (match(*sourceStream, "<?xml")) {
        type = NodeType::Declaration;
        currentName = "xml";
        ignoreWS(*sourceStream);
        while (sourceStream->more() && !match(*sourceStream, "?>")) {
          ignoreWS(*sourceStream);
          if (match(*sourceStream, "?>")) { break; }
          std::string attrName = toUtf8(readName(*sourceStream));
          ignoreWS(*sourceStream);
          if (match(*sourceStream, "=")) {
            ignoreWS(*sourceStream);
            std::string attrVal = parseQuotedValue(*sourceStream, nullptr).getParsed();
            currentAttributes.emplace_back(std::move(attrName), std::move(attrVal));
          }
          ignoreWS(*sourceStream);
        }
        currentDepth = 0;
        return true;
      }
      if (match(*sourceStream, "<?")) {
        type = NodeType::ProcessingInstruction;
        auto [piTarget, piData] = parsePIBody(*sourceStream, false, false);
        currentName = std::move(piTarget);
        currentValue = std::move(piData);
        currentDepth = static_cast<int>(openElements.size());
        return true;
      }
      if (match(*sourceStream, "</")) {
        type = NodeType::ElementEnd;
        currentName = toUtf8(readName(*sourceStream));
        ignoreWS(*sourceStream);
        if (sourceStream->more() && sourceStream->current() == '>') { sourceStream->next(); }
        if (!openElements.empty()) { openElements.pop_back(); }
        currentDepth = static_cast<int>(openElements.size());
        isSelfClosing = false;
        return true;
      }

      // Start Element Tag: '<' Name ... '>' or '/>'
      sourceStream->next(); // skip '<'
      currentName = toUtf8(readName(*sourceStream));
      ignoreWS(*sourceStream);

      // Parse element attributes
      while (sourceStream->more() && sourceStream->current() != '/' && sourceStream->current() != '>') {
        std::string attrName = toUtf8(readName(*sourceStream));
        ignoreWS(*sourceStream);
        if (match(*sourceStream, "=")) {
          ignoreWS(*sourceStream);
          std::string attrVal = parseQuotedValue(*sourceStream, nullptr).getParsed();
          currentAttributes.emplace_back(std::move(attrName), std::move(attrVal));
        }
        ignoreWS(*sourceStream);
      }

      if (match(*sourceStream, "/>")) {
        type = NodeType::ElementStart;
        isSelfClosing = true;
        pendingEndElement = true;
        currentDepth = static_cast<int>(openElements.size()) + 1;
        return true;
      }
      if (match(*sourceStream, ">")) {
        type = NodeType::ElementStart;
        isSelfClosing = false;
        openElements.push_back(currentName);
        currentDepth = static_cast<int>(openElements.size());
        return true;
      }
    } else {
      // Content / Text / Whitespace
      std::string text;
      text.reserve(64);
      bool isAllWhitespace = true;

      while (sourceStream->more() && sourceStream->current() != '<') {
        if (sourceStream->current() == '&') {
          sourceStream->next();
          std::string ent;
          while (sourceStream->more() && sourceStream->current() != ';') {
            ent += static_cast<char>(sourceStream->current());
            sourceStream->next();
          }
          if (sourceStream->more() && sourceStream->current() == ';') { sourceStream->next(); }

          if (ent == "lt") {
            text += '<';
            isAllWhitespace = false;
          } else if (ent == "gt") {
            text += '>';
            isAllWhitespace = false;
          } else if (ent == "amp") {
            text += '&';
            isAllWhitespace = false;
          } else if (ent == "quot") {
            text += '"';
            isAllWhitespace = false;
          } else if (ent == "apos") {
            text += '\'';
            isAllWhitespace = false;
          } else if (!ent.empty() && ent[0] == '#') {
            unsigned long code = 0;
            if (ent.size() > 1 && (ent[1] == 'x' || ent[1] == 'X')) {
              code = std::stoul(ent.substr(2), nullptr, 16);
            } else {
              code = std::stoul(ent.substr(1), nullptr, 10);
            }
            text += toUtf8(static_cast<Char>(code));
            isAllWhitespace = false;
          } else {
            text += '&' + ent + ';';
            isAllWhitespace = false;
          }
        } else {
          const Char ch = sourceStream->current();
          sourceStream->next();
          if (!isWS(ch)) { isAllWhitespace = false; }
          text += toUtf8(ch);
        }
      }

      if (isAllWhitespace && skipWhitespace) { continue; }

      type = isAllWhitespace ? NodeType::Whitespace : NodeType::Text;
      currentValue = std::move(text);
      currentDepth = static_cast<int>(openElements.size());
      return true;
    }
  }

  type = NodeType::EndDocument;
  atEnd = true;
  return false;
}

std::string_view XMLReader::getAttribute(std::string_view attrName) const noexcept
{
  for (const auto &[name, value] : currentAttributes) {
    if (name == attrName) { return value; }
  }
  return {};
}

std::optional<std::string_view> XMLReader::findAttribute(std::string_view attrName) const noexcept
{
  for (const auto &[name, value] : currentAttributes) {
    if (name == attrName) { return value; }
  }
  return std::nullopt;
}

std::expected<bool, XML_Error> XMLReader::readExpected() noexcept
{
  try {
    return read();
  } catch (const std::exception &e) {
    const auto [line, col] = getPosition();
    return std::unexpected(XML_Error{ e.what(), line, col });
  } catch (...) {
    const auto [line, col] = getPosition();
    return std::unexpected(XML_Error{ "Unknown XMLReader error.", line, col });
  }
}

bool XMLReader::hasAttribute(std::string_view attrName) const noexcept
{
  for (const auto &[name, value] : currentAttributes) {
    if (name == attrName) { return true; }
  }
  return false;
}

std::string_view XMLReader::getAttribute(std::size_t index) const
{
  return currentAttributes.at(index).second;
}

std::string_view XMLReader::getAttributeName(std::size_t index) const
{
  return currentAttributes.at(index).first;
}

std::pair<long, long> XMLReader::getPosition() const
{
  return sourceStream ? sourceStream->getPosition() : std::pair<long, long>{ 0, 0 };
}

bool XMLReader::readToNextElement()
{
  while (read()) {
    if (type == NodeType::ElementStart) { return true; }
  }
  return false;
}

std::string XMLReader::readElementText()
{
  std::string accumulated;
  const int targetDepth = currentDepth;
  while (read()) {
    if (type == NodeType::Text || type == NodeType::CDATA) {
      accumulated += currentValue;
    } else if (type == NodeType::ElementEnd && currentDepth < targetDepth) {
      break;
    }
  }
  return accumulated;
}

void XMLReader::skip()
{
  if (isSelfClosing) { return; }
  const int targetDepth = currentDepth;
  while (read()) {
    if (type == NodeType::ElementEnd && currentDepth < targetDepth) { break; }
  }
}

} // namespace XML_Lib
