#pragma once
//
// C++ STL
//
#include <fstream>
#include <string>
#include <stdexcept>
//
// ISource interface
//
#include "ISource.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // XML character constants
    //
    constexpr char kCarriageReturn{0x0D};
    constexpr char kLineFeed{0x0A};
    //
    // Source classes for parsers.
    //
    class BufferSource : public ISource
    {
    public:
        BufferSource() = default;
        explicit BufferSource(const std::u16string &sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
            std::u16string utf16xml{sourceBuffer};
            if (utf16xml.find(u"<?xml") != 0)
            {
                for (char16_t &ch : utf16xml)
                {
                    ch = (static_cast<uint16_t>(ch) >> 8) | (static_cast<uint16_t>(ch) << 8);
                }
            }
            m_parseBuffer = m_UTF8.from_bytes(m_UTF16.to_bytes(utf16xml));
            convertCRLFToLF(m_parseBuffer);
        }
        explicit BufferSource(const std::string &sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
            m_parseBuffer = m_UTF8.from_bytes(sourceBuffer);
            convertCRLFToLF(m_parseBuffer);
        }
        [[nodiscard]] ISource::Char current() const override
        {
            if (more())
            {
                return (m_parseBuffer[m_bufferPosition]);
            }
            return (static_cast<ISource::Char>(EOF));
        }
        void next() override
        {
            if (!more())
            {
                throw std::runtime_error("Parse buffer empty before parse complete.");
            }
            m_bufferPosition++;
            m_column++;
            if (current() == kLineFeed)
            {
                m_lineNo++;
                m_column = 1;
            }
        }
        [[nodiscard]] bool more() const override
        {
            return (m_bufferPosition < static_cast<long>(m_parseBuffer.size()));
        }
        void backup(long length) override
        {
            m_bufferPosition -= length;
            if (m_bufferPosition < 0)
            {
                m_bufferPosition = 0;
            }
        }
        [[nodiscard]] long position() const override
        {
            return (m_bufferPosition);
        }
        std::string getRange(long start, long end) override
        {
            return (m_UTF8.to_bytes(m_parseBuffer.substr(start, end - start)));
        }
        void reset() override
        {
            m_lineNo = 1;
            m_column = 1;
            m_bufferPosition = 0;
        }

    private:
        void convertCRLFToLF(ISource::String &xmlString)
        {
            size_t pos = xmlString.find(U"\x0D\x0A");
            while (pos != std::string::npos)
            {
                xmlString.replace(pos, 2, U"\x0A");
                pos = xmlString.find(U"\x0D\x0A", pos + 1);
            }
        }
        long m_bufferPosition = 0;
        ISource::String m_parseBuffer;
    };
    class FileSource : public ISource
    {
    public:
        explicit FileSource(const std::string &sourceFileName)
        {
            m_source.open(sourceFileName.c_str(), std::ios_base::binary);
            if (!m_source.is_open())
            {
                throw std::runtime_error("XML file input stream failed to open or does not exist.");
            }
            if (current() == kCarriageReturn)
            {
                m_source.get();
                if (current() != kLineFeed)
                {
                    m_source.unget();
                }
            }
        }
        ISource::Char current() const override
        {
            return (m_source.peek());
        }
        void next() override
        {
            if (!more())
            {
                throw std::runtime_error("Parse buffer empty before parse complete.");
            }
            m_source.get();
            if (current() == kCarriageReturn)
            {
                m_source.get();
                if (current() != kLineFeed)
                {
                    m_source.unget();
                }
            }
            m_column++;
            if (current() == kLineFeed)
            {
                m_lineNo++;
                m_column = 1;
            }
        }
        bool more() const override
        {
            return (m_source.peek() != EOF);
        }
        void backup(long length) override
        {
            if ((static_cast<long>(m_source.tellg()) - length >= 0) || (current() == (ISource::Char)EOF))
            {
                m_source.clear();
                m_source.seekg(-length, std::ios_base::cur);
            }
            else
            {
                m_source.seekg(0, std::ios_base::beg);
            }
        }
        long position() const override
        {
            return (static_cast<long>(m_source.tellg()));
        }
        void reset() override
        {
            m_lineNo = 1;
            m_column = 1;
            m_source.clear();
            m_source.seekg(0, std::ios_base::beg);
        }
        std::string getRange(long start, long end) override
        {
            std::string rangeBuffer(end - start, ' ');
            long currentPosition = (long)m_source.tellg();
            m_source.seekg(start, std::ios_base::beg);
            m_source.read(&rangeBuffer[0], end - start);
            m_source.seekg(currentPosition, std::ios_base::beg);
            return (rangeBuffer);
        }

    private:
        mutable std::ifstream m_source;
    };
} // namespace XMLLib