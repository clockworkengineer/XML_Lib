#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <fstream>
//
// IDestination interface
//
#include "IDestination.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // Destination classes for stringification.
    //
    class BufferDestination : public IDestination
    {
    public:
        BufferDestination() = default;
        void add(const std::string &bytes) override
        {
            for (auto b : bytes)
            {
                m_stringifyBuffer.push_back(b);
            }
        }
        void add(const ISource::Char ch) override
        {
            add(m_UTF8.to_bytes(ch));
        }
        [[nodiscard]] std::string getBuffer() const
        {
            return (m_stringifyBuffer);
        }

    private:
        std::string m_stringifyBuffer;
    };
    class FileDestination : public IDestination
    {
    public:
        explicit FileDestination(const std::string &destinationFileName)
        {
            m_destination.open(destinationFileName.c_str(), std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("XML file output stream failed to open or could not be created.");
            }
        }
        void add(const ISource::Char ch) override
        {
            add(m_UTF8.to_bytes(ch));
        }
        void add(const std::string &bytes) override
        {
            m_destination.write(bytes.c_str(), bytes.length());
            m_destination.flush();
        }

    private:
        std::ofstream m_destination;
    };
} // namespace XMLLib