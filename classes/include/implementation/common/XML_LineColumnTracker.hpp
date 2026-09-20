#pragma once

#include "XML_Types.hpp"
#include "interface/ISource.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace XML_Lib {

/// @brief Utility component for accurate line and column position tracking within character streams.
/// Supports position unwinding during backup operations.
class LineColumnTracker
{
public:
  LineColumnTracker() { reset(); }

  /// @brief Reset position tracking to the beginning (line 1, column 1).
  void reset()
  {
    lineNo = 1;
    columnNo = 1;
    lineStarts.clear();
    lineStarts.push_back(0);
  }

  /// @brief Record advancing stream position to character @p currentCh at byte offset @p currentBytePos.
  void advance(Char currentCh, long currentBytePos)
  {
    columnNo++;
    if (currentCh == kLineFeed) {
      lineNo++;
      columnNo = 1;
      if (lineStarts.empty() || lineStarts.back() < currentBytePos) {
        lineStarts.push_back(currentBytePos);
      }
    }
  }

  /// @brief Adjust line and column positions when rewinding stream position to @p newBytePos.
  void rewindTo(long newBytePos)
  {
    if (newBytePos <= 0) {
      lineNo = 1;
      columnNo = 1;
      return;
    }
    const auto it = std::upper_bound(lineStarts.begin(), lineStarts.end(), newBytePos);
    const std::size_t idx = static_cast<std::size_t>(it - lineStarts.begin());
    lineNo = static_cast<long>(idx);
    const long lineStart = (idx > 0) ? lineStarts[idx - 1] : 0;
    columnNo = (newBytePos - lineStart) + 1;
  }

  [[nodiscard]] long getLineNumber() const noexcept { return lineNo; }
  [[nodiscard]] long getColumnNumber() const noexcept { return columnNo; }
  [[nodiscard]] std::pair<long, long> getPosition() const noexcept { return std::make_pair(lineNo, columnNo); }

private:
  long lineNo = 1;
  long columnNo = 1;
  std::vector<long> lineStarts;
};

}// namespace XML_Lib
