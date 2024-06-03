#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <stdexcept>

#include "plog/Initializers/RollingFileInitializer.h"
#include "plog/Log.h"

class Utility
{
public:
  static std::vector<std::string> createXMLFileList()
  {
    std::vector<std::string> fileList;
    for (auto &file : std::filesystem::directory_iterator((std::filesystem::current_path() / "files"))) {
      if (file.path().extension() == ".xml") { fileList.push_back(file.path().string()); }
    }
    return (fileList);
  }
  static std::string createFileName(const std::string &torrentFileName, const std::string &newExtension)
  {
    std::string newFileName = torrentFileName;
    return (newFileName.erase(newFileName.find(".xml"), newFileName.length()) + newExtension);
  }
};
