#pragma once

#include <iostream>
#include "XML.hpp"
#include "XML_Core.hpp"

class Utility
{
public:
  /// @brief
  /// Create a list of XML files in the sample files directory.
  
  static std::vector<std::string> createXMLFileList()
  {
    std::vector<std::string> fileList;
    for (auto &file : std::filesystem::directory_iterator((std::filesystem::current_path() / "files"))) {
      if (file.path().extension() == ".xml") { fileList.push_back(file.path().string()); }
    }
    return (fileList);
  }
  /// @brief
  /// Generate a new file name by replacing the .xml extension.
  
  static std::string createFileName(const std::string &torrentFileName, const std::string &newExtension)
  {
    std::string newFileName = torrentFileName;
    return (newFileName.erase(newFileName.find(".xml"), newFileName.length()) + newExtension);
  }
};
