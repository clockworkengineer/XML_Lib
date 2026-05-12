#pragma once
// stdafx.h — Windows/MSVC precompiled header.
// Used only when building with MSVC (via target_precompile_headers in
// CMakeLists.txt with an if(MSVC) guard).  On GCC/Clang the compiler's own
// unity-build / module caching makes an explicit PCH unnecessary, and the
// non-MSVC builds rely on the compiler's built-in header caching instead.

#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <filesystem>
#include <set>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <cwctype>
#include <cstring>
#include <regex>
#include <ranges>
#include <functional>
#include <sstream>
