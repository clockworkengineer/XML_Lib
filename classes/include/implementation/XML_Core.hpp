#pragma once

// Internal aggregate — do not include in new files.
// Each new implementation file should include only the specific headers it needs.
// This header exists for files that genuinely require broad access to all XML_Lib
// internals (e.g. Default_Parser.cpp, large Impl headers).
//
// ==========
// XML Core
// ==========
#include "XML_Config.hpp"
#include "common/XML_Character.hpp"
#include "XML_Interfaces.hpp"
#include "common/XML_Error.hpp"
#include "common/XML_Utility.hpp"
#include "common/XML_Arena.hpp"
#include "converter/XML_Converter.hpp"
#include "data/XML_Value.hpp"
#include "data/XML_Attribute.hpp"
#include "data/XML_ExternalReference.hpp"
#include "nodes/XML_Variant.hpp"
#include "node/XML_Node.hpp"
#include "nodes/XML_Prolog.hpp"
#include "nodes/XML_Declaration.hpp"
#include "nodes/XML_Content.hpp"
#include "nodes/XML_CDATA.hpp"
#include "nodes/XML_EntityReference.hpp"
#include "nodes/XML_Element.hpp"
#include "nodes/XML_Root.hpp"
#include "nodes/XML_Self.hpp"
#include "nodes/XML_Comment.hpp"
#include "nodes/XML_PI.hpp"
#include "nodes/XML_DTD.hpp"
#include "node/XML_Node_Contents.hpp"
#include "node/XML_Node_Reference.hpp"
#include "node/XML_Node_Index.hpp"
#include "node/XML_Node_Creation.hpp"
#include "entity/XML_EntityMapping.hpp"
#include "entity/XML_EntityMapper.hpp"
#include "io/XML_Sources.hpp"
#include "io/XML_Destinations.hpp"
#include "common/XML_Parse.hpp"
#include "common/XML_SourceHelpers.hpp"
#include "parser/Default_Parser.hpp"
#if defined(XML_LIB_ENABLE_STRINGIFY)
#include "stringify/Default_Stringify.hpp"
#endif