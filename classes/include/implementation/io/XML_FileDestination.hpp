#pragma once
#include "common/XML_Error.hpp"
#include "XML_Converter.hpp"

#include "IDestination.hpp"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

class FileDestination final : public IDestination
{
public:
  // FileDestination Error
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("FileDestination");
#endif
  // Constructors/Destructors
  explicit FileDestination(const std::string_view &filename) : filename(filename)
  {
    destination.open(filename.data(), std::ios_base::binary);
    if (!destination.is_open()) { XML_LIB_THROW(Error("File output stream failed to open or could not be created.")); }
  }
  FileDestination() = default;
  FileDestination(const FileDestination &other) = delete;
  FileDestination &operator=(const FileDestination &other) = delete;
  FileDestination(FileDestination &&other) = delete;
  FileDestination &operator=(FileDestination &&other) = delete;
  ~FileDestination() override = default;

  void add(const Char c) override { add(toUtf8(c)); }
  void add(const std::string &bytes) override
  {
    destination.write(bytes.c_str(), static_cast<std::streamsize>(bytes.length()));
    destination.flush();
    fileSize += bytes.length();
  }
  void add(const char *bytes) override
  {
    destination.write(bytes, static_cast<std::streamsize>(strlen(bytes)));
    destination.flush();
    fileSize += strlen(bytes);
  }
  void add(const std::string_view &bytes) override
  {
    destination.write(bytes.data(), static_cast<std::streamsize>(bytes.length()));
    destination.flush();
    fileSize += bytes.length();
  }
  void clear() override
  {
    if (destination.is_open()) { destination.close(); }
    destination.open(filename.c_str(), std::ios_base::binary | std::ios_base::trunc);
    if (!destination.is_open()) { XML_LIB_THROW(Error("File output stream failed to open or could not be created.")); }
    fileSize = 0;
  }
  std::size_t size() const { return fileSize; }
  std::string getFileName() { return filename; }
  void close() { destination.close(); }

private:
  std::ofstream destination;
  std::string filename;
  std::size_t fileSize{};
};
}// namespace XML_Lib