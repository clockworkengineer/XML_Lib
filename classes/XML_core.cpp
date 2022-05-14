//
// Class: XML
//
// Description: XML Parser core functionality.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "XML_core.hpp"
#include "XML_errors.hpp"
#include "XML_sources.hpp"
#include "XML_destinations.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    /// <summary>
    /// Check whether a character is valid for XML.
    /// </summary>
    /// <param name="ch">Character to validate.</param>
    /// <returns>true then valid otherwise false.</returns>
    bool validChar(ISource::Char ch)
    {
        return ((ch == 0x09) ||
                (ch == kLineFeed) ||
                (ch == kCarriageReturn) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
    /// <summary>
    /// Check whether character is a valid to start an XML name with.
    /// </summary>
    /// <param name="ch">Character value to validate.</param>
    /// <returns>true then valid otherwise false.</returns>
    bool validNameStartChar(ISource::Char ch)
    {
        return ((ch == ':') ||
                (ch == '_') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 0xC0 && ch <= 0xD6) ||
                (ch >= 0xD8 && ch <= 0xF6) ||
                (ch >= 0xF8 && ch <= 0x2FF) ||
                (ch >= 0x370 && ch <= 0x37D) ||
                (ch >= 0x37F && ch <= 0x1FFF) ||
                (ch >= 0x200C && ch <= 0x200D) ||
                (ch >= 0x2070 && ch <= 0x218F) ||
                (ch >= 0x2C00 && ch <= 0x2FEF) ||
                (ch >= 0x3001 && ch <= 0xD7FF) ||
                (ch >= 0xF900 && ch <= 0xFDCF) ||
                (ch >= 0xFDF0 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0xEFFFF));
    }
    /// <summary>
    /// Check whether a character is valid for an XML name.
    /// </summary>
    /// <param name="ch">Name to validate.</param>
    /// <returns>true then valid otherwise false.</returns>
    bool validNameChar(ISource::Char ch)
    {
        return (validNameStartChar(ch) ||
                (ch == '-') ||
                (ch == '.') ||
                (ch >= '0' && ch <= '9') ||
                (ch == 0xB7) ||
                (ch >= 0x0300 && ch <= 0x036F) ||
                (ch >= 0x203F && ch <= 0x2040));
    }
    /// <summary>
    /// Check name that starts with xml is a valid reserved name.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>true then valid otherwise false.</returns>
    bool validReservedName(const ISource::String &name)
    {
        return ((name.find(U"xmlns") == 0) || (name.find(U"xml-stylesheet") == 0) || (name == U"xml"));
    }
    /// <summary>
    /// Validate XML tag/attribute names.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>true then valid otherwise false.</returns>
    bool validName(ISource::String name)
    {
        if (name.empty())
        {
            return (false);
        }
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.find(U"xml") == 0 && !(validReservedName(name)))
        {
            return (false);
        }
        if (!validNameStartChar(name[0]))
        {
            return (false);
        }
        for (auto it = name.begin() + 1; it != name.end(); it++)
        {
            if (!validNameChar(*it))
            {
                return (false);
            }
        }
        return (true);
    }
    /// <summary>
    /// Make sure that XML attribute value does not contain any illegal characters.
    /// </summary>
    /// <param name=""></param>
    /// <returns>true then contains all legal characters otherwise false.</returns>
    bool validAttributeValue(XMLValue &value)
    {
        BufferSource valueSource(value.parsed);
        while (valueSource.more())
        {
            if (valueSource.match(U"&#"))
            {
                parseCharacterReference(valueSource);
            }
            else if (valueSource.current() == '&')
            {
                parseEntityReference(valueSource);
            }
            else if ((valueSource.current() == '<') ||
                     (valueSource.current() == '"') ||
                     (valueSource.current() == '\''))
            {
                return (false);
            }
            else
            {
                valueSource.next();
            }
        }
        return (true);
    }
    /// <summary>
    /// Parse  and return XML name.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>XML name.</returns>
    std::string parseName(ISource &source)
    {
        ISource::String name;
        while (source.more() && validNameChar(source.current()))
        {
            name += source.current();
            source.next();
        }
        source.ignoreWS();
        if (!validName(name))
        {
            throw SyntaxError(source, "Invalid name '" + source.to_bytes(name) + "' encountered.");
        }
        return (source.to_bytes(name));
    }
    /// <summary>
    /// Parse and return XML entity reference.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>Parsed entity reference value.</returns>
    XMLValue parseEntityReference(ISource &source)
    {
        XMLValue entityReference;
        entityReference.unparsed = source.current_to_bytes();
        source.next();
        entityReference.unparsed += parseName(source);
        if (source.current() != ';')
        {
            throw SyntaxError(source, "Invalidly formed entity reference.");
        }
        entityReference.unparsed += ';';
        source.next();
        return (entityReference);
    }
    /// <summary>
    /// Parse a character reference value (hex/dec) returning its value.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>Character reference value.</returns>
    XMLValue parseCharacterReference(ISource &source)
    {
        XMLValue characterRefence;
        characterRefence.unparsed = "&#";
        while (source.more() && source.current() != ';')
        {
            characterRefence.unparsed += source.current_to_bytes();
            source.next();
        }
        if (source.current() != ';')
        {
            throw SyntaxError(source, "Invalidly formed  character reference.");
        }
        source.next();
        characterRefence.unparsed += ';';
        std::string reference = characterRefence.unparsed;
        char *end;
        long result = 10;
        if (reference[2] == 'x')
        {
            reference = reference.substr(3, reference.size() - 4);
            result = 16;
        }
        else
        {
            reference = reference.substr(2, reference.size() - 3);
        }
        result = std::strtol(reference.c_str(), &end, result);
        if (*end == '\0')
        {
            if (!validChar(result))
            {
                throw SyntaxError(source, "Character reference invalid character.");
            }
            characterRefence.parsed = source.to_bytes(result);
            return (characterRefence);
        }
        throw SyntaxError(source, "Cannot convert character reference.");
    }
    /// <summary>
    /// Parse character value which can be either be a plain character,
    /// character reference or entity reference that maps to a string of
    /// characters.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>Character value.</returns>
    XMLValue parseCharacter(ISource &source)
    {
        XMLValue character;
        if (source.match(U"&#"))
        {
            character = parseCharacterReference(source);
        }
        else if (source.current() == '&')
        {
            character = parseEntityReference(source);
            character.parsed = character.unparsed;
        }
        else if (validChar(source.current()))
        {
            character.parsed = source.current_to_bytes();
            character.unparsed = character.parsed;
            source.next();
        }
        else
        {
            throw SyntaxError(source, "Invalid character value encountered.");
        }
        return (character);
    }
    /// <summary>
    /// Parse literal string value and return it.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="entityMapper">Entity mapper.</param>
    /// <returns>Literal string value.</returns>
    XMLValue parseValue(ISource &source, IXMLEntityMapper &entityMapper)
    {
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XMLValue value;
            ISource::Char quote = source.current();
            source.next();
            while (source.more() && source.current() != quote)
            {
                XMLValue character = parseCharacter(source);
                if (character.isEntityReference())
                {
                    entityMapper.map(character);
                }
                value += character;
            }
            source.next();
            source.ignoreWS();
            return (value);
        }
        throw SyntaxError(source, "Invalid attribute value.");
    }
    /// <summary>
    /// Parse literal string value and return it.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>Literal string value.</returns>
    XMLValue parseValue(ISource &source)
    {
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XMLValue value;
            ISource::Char quote = source.current();
            source.next();
            while (source.more() && source.current() != quote)
            {
                value += parseCharacter(source);
            }
            source.next();
            source.ignoreWS();
            return (value);
        }
        throw SyntaxError(source, "Invalid attribute value.");
    }
    /// <summary>
    /// Extract body of tag up until '>'.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <returns>Body of tag.</returns>
    std::string parseTagBody(ISource &source)
    {
        source.ignoreWS();
        std::string body;
        while (source.more() && source.current() != '>')
        {
            body += source.current_to_bytes();
            source.next();
        }
        return (body);
    }
    /// <summary>
    /// Split a string into a vector of strings using the passed in delimeter.
    /// </summary>
    /// <param name="stringToSplit">String to split up.</param>
    /// <param name="delimeter">Character delimeter to split on.</param>
    /// <returns>Vector of split strings.</returns>
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter)
    {
        std::stringstream sourceStream(stringToSplit);
        std::string splitOffItem;
        std::vector<std::string> splitStrings;
        while (std::getline(sourceStream, splitOffItem, delimeter))
        {
            splitStrings.push_back(splitOffItem);
        }
        return splitStrings;
    }
    /// <summary>
    /// Trimm whitespace from beginning and end of a string.
    /// </summary>
    /// <param name="stringToTrimm">String to trimm.</param>
    void trimmString(std::string &stringToTrimm)
    {
        stringToTrimm.erase(stringToTrimm.begin(), std::find_if(stringToTrimm.begin(), stringToTrimm.end(), [](unsigned char ch)
                                                                { return !std::iswspace(ch); }));
        stringToTrimm.erase(std::find_if(stringToTrimm.rbegin(), stringToTrimm.rend(), [](unsigned char ch)
                                         { return !std::iswspace(ch); })
                                .base(),
                            stringToTrimm.end());
    }
    /// <summary>
    /// Trimm whitespace from beginning and end of a string.
    /// </summary>
    /// <param name="stringToTrimm">String to trimm.</param>
    void toUpperString(std::string &stringToUpper)
    {
        std::transform(stringToUpper.begin(), stringToUpper.end(),
                       stringToUpper.begin(), [](unsigned int c)
                       { return static_cast<char>(std::toupper(c)); });
    }
} // namespace XMLLib
