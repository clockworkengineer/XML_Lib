#pragma once

#include <algorithm>
#include <cwctype>
#include <cstring>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "XML_Types.hpp"
#include "ISource.hpp"
#include "IDestination.hpp"
#include "IEntityMapper.hpp"
#include "IValidator.hpp"
#include "IParser.hpp"
#include "IStringify.hpp"
#include "IAction.hpp"