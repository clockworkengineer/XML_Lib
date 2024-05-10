#pragma once

#include <string>
#include <stdexcept>
#include <fstream>

#include "IDestination.hpp"

namespace XML_Lib {

class FileDestination : public IDestination
{
public:
  // FileDestination Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("FileDestination Error: " + message) {}
  };
  // Constructors/Destructors
  explicit FileDestination(const std::string &destinationFileName)
  {
    destination.open(destinationFileName.c_str(), std::ios_base::binary);
    if (!destination.is_open()) { throw Error("File output stream failed to open or could not be created."); }
  }
  FileDestination() = default;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() = default;

  void add(Char c) override { add(toUtf8(c)); }
  void add(const std::string &bytes) override
  {
    destination.write(bytes.c_str(), static_cast<std::streamsize>(bytes.length()));
    destination.flush();
  }

private:
  std::ofstream destination;
};
}// namespace XML_Lib