#pragma once
//
// C++ STL
//
#include <string>
#include <codecvt>
#include <locale>
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // Interface for writing destination stream during XML/DTD stringification.
    //
    class IDestination
    {
    public:
        // Virtual destructor
        virtual ~IDestination() {}
        // Add bytes to destination
        virtual void add(const std::string &bytes) = 0;
        virtual void add(const ISource::Char) = 0;

    protected:
        std::wstring_convert<std::codecvt_utf8_utf16<ISource::String::value_type>, ISource::String::value_type> m_UTF8;
    };
} // namespace XMLLib