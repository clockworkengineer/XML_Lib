#pragma once
// =======
// C++ STL
// =======
#include <fstream>
#include <string>
#include <stdexcept>
// ================
// Source interface
// ================
#include "ISource.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class FileSource : public ISource
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit FileSource(const std::string &sourceFileName)
  {
    m_source.open(sourceFileName.c_str(), std::ios_base::binary);
    if (!m_source.is_open()) { throw Error("File input stream failed to open or does not exist."); }
    if (current() == kCarriageReturn) {
      m_source.get();
      if (current() != kLineFeed) { m_source.unget(); }
    }
  }
  FileSource() = default;
  FileSource(const FileSource &other) = delete;
  FileSource &operator=(const FileSource &other) = delete;
  FileSource(FileSource &&other) = delete;
  FileSource &operator=(FileSource &&other) = delete;
  ~FileSource() = default;
  // ==============
  // PUBLIC METHODS
  // ==============
  ISource::Char current() const override { return (m_source.peek()); }
  void next() override
  {
    if (!more()) { throw Error("Parse buffer empty before parse complete."); }
    m_source.get();
    if (current() == kCarriageReturn) {
      m_source.get();
      if (current() != kLineFeed) { m_source.unget(); }
    }
    m_column++;
    if (current() == kLineFeed) {
      m_lineNo++;
      m_column = 1;
    }
  }
  bool more() const override { return (m_source.peek() != EOF); }
  void backup(long length) override
  {
    if ((static_cast<long>(m_source.tellg()) - length >= 0) || (current() == (ISource::Char)EOF)) {
      m_source.clear();
      m_source.seekg(-length, std::ios_base::cur);
    } else {
      m_source.seekg(0, std::ios_base::beg);
    }
  }
  long position() const override { return (static_cast<long>(m_source.tellg())); }
  void reset() override
  {
    m_lineNo = 1;
    m_column = 1;
    m_source.clear();
    m_source.seekg(0, std::ios_base::beg);
  }
  std::string getRange(long start, long end) override
  {
    std::string rangeBuffer(static_cast<std::size_t>(end) - start, ' ');
    long currentPosition = (long)m_source.tellg();
    m_source.seekg(start, std::ios_base::beg);
    m_source.read(&rangeBuffer[0], static_cast<std::streamsize>(end) - start);
    m_source.seekg(currentPosition, std::ios_base::beg);
    return (rangeBuffer);
  }
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===============
  // PRIVATE METHODS
  // ===============
  // =================
  // PRIVATE VARIABLES
  // =================
  mutable std::ifstream m_source;
};
}// namespace XML_Lib