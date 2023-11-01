#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

#include "IDestination.hpp"

namespace XML_Lib {

class FileDestination : public IDestination
{
public:
  explicit FileDestination(const std::string &destinationFileName)
  {
    m_destination.open(destinationFileName.c_str(), std::ios_base::binary);
    if (!m_destination.is_open()) { throw Error("File output stream failed to open or could not be created."); }
  }
  FileDestination() = default;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() = default;

  void add(XML_Lib::Char c) override { add(toUtf8(c)); }
  void add(const std::string &bytes) override
  {
    m_destination.write(bytes.c_str(), static_cast<std::streamsize>(bytes.length()));
    m_destination.flush();
  }

private:
  std::ofstream m_destination;
};
}// namespace XML_Lib