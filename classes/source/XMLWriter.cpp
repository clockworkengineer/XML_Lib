//
// Class: XMLWriter
//
// Description: Streaming XML serializer.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XMLWriter.hpp"
#include "common/XML_Error.hpp"
#include <stdexcept>

namespace XML_Lib {

static void escapeAndWriteText(IDestination &dest, std::string_view text)
{
  for (const char ch : text) {
    switch (ch) {
    case '&':
      dest.add("&amp;");
      break;
    case '<':
      dest.add("&lt;");
      break;
    case '>':
      dest.add("&gt;");
      break;
    case '"':
      dest.add("&quot;");
      break;
    case '\'':
      dest.add("&apos;");
      break;
    default:
      dest.add(ch);
      break;
    }
  }
}

XMLWriter::XMLWriter(IDestination &destination) : dest(&destination) {}

XMLWriter::XMLWriter()
  : ownedBufferDest(std::make_unique<BufferDestination>())
{
  dest = ownedBufferDest.get();
}

XMLWriter XMLWriter::toFile(const std::filesystem::path &filePath)
{
  XMLWriter writer;
  writer.ownedFileDest = std::make_unique<FileDestination>(filePath.string());
  writer.dest = writer.ownedFileDest.get();
  writer.ownedBufferDest.reset();
  return writer;
}

XMLWriter::XMLWriter(XMLWriter &&other) noexcept
  : ownedBufferDest(std::move(other.ownedBufferDest))
  , ownedFileDest(std::move(other.ownedFileDest))
  , dest(other.dest == other.ownedBufferDest.get()
           ? nullptr
           : (other.dest == other.ownedFileDest.get() ? nullptr : other.dest))
  , openElements(std::move(other.openElements))
  , inStartTag(other.inStartTag)
  , indentEnabled(other.indentEnabled)
  , indentSpaces(other.indentSpaces)
  , currentIndentLevel(other.currentIndentLevel)
  , omitDeclaration(other.omitDeclaration)
  , documentStarted(other.documentStarted)
  , lastWasStartElement(other.lastWasStartElement)
{
  if (!dest) {
    if (ownedBufferDest) {
      dest = ownedBufferDest.get();
    } else if (ownedFileDest) {
      dest = ownedFileDest.get();
    }
  }
}

XMLWriter &XMLWriter::operator=(XMLWriter &&other) noexcept
{
  if (this != &other) {
    ownedBufferDest = std::move(other.ownedBufferDest);
    ownedFileDest = std::move(other.ownedFileDest);
    if (other.dest == other.ownedBufferDest.get()) {
      dest = ownedBufferDest.get();
    } else if (other.dest == other.ownedFileDest.get()) {
      dest = ownedFileDest.get();
    } else {
      dest = other.dest;
    }
    openElements = std::move(other.openElements);
    inStartTag = other.inStartTag;
    indentEnabled = other.indentEnabled;
    indentSpaces = other.indentSpaces;
    currentIndentLevel = other.currentIndentLevel;
    omitDeclaration = other.omitDeclaration;
    documentStarted = other.documentStarted;
    lastWasStartElement = other.lastWasStartElement;
  }
  return *this;
}

XMLWriter::~XMLWriter()
{
  if (dest) {
#ifndef XML_LIB_NO_EXCEPTIONS
    try {
      writeEndDocument();
    } catch (...) {
      // Destructors must not throw
    }
#else
    writeEndDocument();
#endif
  }
}

void XMLWriter::setIndent(bool enable, int spaces) noexcept
{
  indentEnabled = enable;
  indentSpaces = (spaces > 0) ? spaces : 2;
}

void XMLWriter::closePendingStartTag(bool emptyElement)
{
  if (!dest || !inStartTag) { return; }
  if (emptyElement) {
    dest->add("/>");
  } else {
    dest->add(">");
  }
  inStartTag = false;
}

void XMLWriter::writeIndentation()
{
  if (!dest || !indentEnabled) { return; }
  const int totalSpaces = currentIndentLevel * indentSpaces;
  for (int i = 0; i < totalSpaces; ++i) { dest->add(' '); }
}

void XMLWriter::writeStartDocument(std::string_view version, std::string_view encoding, bool standalone)
{
  if (!dest || omitDeclaration) { return; }
  dest->add("<?xml version=\"");
  dest->add(version);
  dest->add("\" encoding=\"");
  dest->add(encoding);
  dest->add("\"");
  if (standalone) { dest->add(" standalone=\"yes\""); }
  dest->add("?>");
  documentStarted = true;
}

void XMLWriter::writeStartElement(std::string_view name)
{
  if (!dest) { return; }
  closePendingStartTag(false);

  if (!openElements.empty()) {
    openElements.back().hasChildElements = true;
  }

  if (indentEnabled) {
    if (documentStarted || !openElements.empty()) { dest->add("\n"); }
    writeIndentation();
  }

  dest->add("<");
  dest->add(name);
  openElements.push_back(ElementEntry{ std::string(name), false });
  inStartTag = true;
  lastWasStartElement = true;
  ++currentIndentLevel;
}

void XMLWriter::writeAttribute(std::string_view name, std::string_view value)
{
  if (!dest || !inStartTag) {
    XML_LIB_THROW(std::runtime_error("XMLWriter Error: writeAttribute must be called directly after writeStartElement."));
  }
  dest->add(" ");
  dest->add(name);
  dest->add("=\"");
  escapeAndWriteText(*dest, value);
  dest->add("\"");
}

void XMLWriter::writeEndElement()
{
  if (!dest || openElements.empty()) { return; }

  ElementEntry entry = std::move(openElements.back());
  openElements.pop_back();
  --currentIndentLevel;

  if (inStartTag) {
    // Self-closing element
    dest->add("/>");
    inStartTag = false;
    lastWasStartElement = false;
    return;
  }

  if (indentEnabled && entry.hasChildElements) {
    dest->add("\n");
    writeIndentation();
  }

  dest->add("</");
  dest->add(entry.name);
  dest->add(">");
  lastWasStartElement = false;
}

void XMLWriter::writeEmptyElement(std::string_view name)
{
  writeStartElement(name);
  writeEndElement();
}

void XMLWriter::writeElement(std::string_view name, std::string_view textContent)
{
  writeStartElement(name);
  writeCharacters(textContent);
  writeEndElement();
}

void XMLWriter::writeCharacters(std::string_view text)
{
  if (!dest) { return; }
  closePendingStartTag(false);
  escapeAndWriteText(*dest, text);
  lastWasStartElement = false;
}

void XMLWriter::writeComment(std::string_view comment)
{
  if (!dest) { return; }
  closePendingStartTag(false);
  if (indentEnabled) {
    if (documentStarted || !openElements.empty()) { dest->add("\n"); }
    writeIndentation();
  }
  dest->add("<!--");
  dest->add(comment);
  dest->add("-->");
  lastWasStartElement = false;
}

void XMLWriter::writeCDATA(std::string_view cdata)
{
  if (!dest) { return; }
  closePendingStartTag(false);
  dest->add("<![CDATA[");
  dest->add(cdata);
  dest->add("]]>");
  lastWasStartElement = false;
}

void XMLWriter::writeProcessingInstruction(std::string_view target, std::string_view data)
{
  if (!dest) { return; }
  closePendingStartTag(false);
  if (indentEnabled) {
    if (documentStarted || !openElements.empty()) { dest->add("\n"); }
    writeIndentation();
  }
  dest->add("<?");
  dest->add(target);
  if (!data.empty()) {
    dest->add(" ");
    dest->add(data);
  }
  dest->add("?>");
  lastWasStartElement = false;
}

void XMLWriter::writeRaw(std::string_view rawMarkup)
{
  if (!dest) { return; }
  closePendingStartTag(false);
  dest->add(rawMarkup);
  lastWasStartElement = false;
}

void XMLWriter::writeEndDocument()
{
  if (!dest) { return; }
  while (!openElements.empty()) { writeEndElement(); }
  closePendingStartTag(false);
}

void XMLWriter::flush()
{
  closePendingStartTag(false);
}

std::string XMLWriter::result() const
{
  if (ownedBufferDest) { return ownedBufferDest->toString(); }
  return {};
}

} // namespace XML_Lib
