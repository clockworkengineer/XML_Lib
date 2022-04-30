#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <fstream>
namespace H4
{
    //
    // Destination classes for stringification.
    //
    class BufferDestination : public IDestination
    {
    public:
        BufferDestination()
        {
        }
        void add(const std::string &bytes) override
        {
            for (auto b : bytes)
            {
                m_stringifyBuffer.push_back(b);
            }
        }
        void add(const ISource::Char ch)  override 
        {
            add(m_UTF8.to_bytes(ch));
        }
        std::string getBuffer() const
        {
            return (m_stringifyBuffer);
        }

    private:
        std::string m_stringifyBuffer;
    };
    class FileDestination : public IDestination
    {
    public:
        FileDestination(const std::string &desinationFileName)
        {
            m_destination.open(desinationFileName.c_str(), std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("XML file output stream failed to open or could not be created.");
            }
        }
        void add(const ISource::Char ch)  override 
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
} // namespace H4