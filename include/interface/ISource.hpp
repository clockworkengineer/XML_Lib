#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <codecvt>
#include <locale>
#include <cwctype>
// =========
// NAMESPACE
// =========
namespace XML_Lib
{
    // ==========================================================
    // Interface for reading source stream during XML/DTD parsing
    // ==========================================================
    class ISource
    {
    public:
        // ============================
        // 32 bit characters internally
        // ============================
        using String = std::u32string;
        using Char = String::value_type;
        // =============
        // ISource Error
        // =============
        struct Error : public std::runtime_error
        {
            Error(std::string const &message) : std::runtime_error("ISource Error: " + message)
            {
            }
        };
        // ========================
        // Constructors/destructors
        // ========================
        ISource() = default;
        ISource(const ISource &other) = delete;
        ISource &operator=(const ISource &other) = delete;
        ISource(ISource &&other) = delete;
        ISource &operator=(ISource &&other) = delete;
        virtual ~ISource() = default;
        // =================
        // Current character
        // =================
        [[nodiscard]] virtual Char current() const = 0;
        // ======================
        // Move to next character
        // ======================
        virtual void next() = 0;
        // =========================================
        // Are there still more characters to read ?
        // =========================================
        [[nodiscard]] virtual bool more() const = 0;
        // ======================
        // Back length characters
        // ======================
        virtual void backup(long length) = 0;
        // ===============================================
        // Current character position within source stream
        // ===============================================
        [[nodiscard]] virtual long position() const = 0;
        // =========================================================
        // Return range of characters as a string from source stream
        // =========================================================
        virtual std::string getRange(long start, long end) = 0;
        // ===================================
        // Reset to beginning of source stream
        // ===================================
        virtual void reset() = 0;
        // =====================================
        // Is the current character whitespace ?
        // =====================================
        [[nodiscard]] bool isWS() const
        {
            return (std::iswspace(static_cast<wint_t>(current()))) != 0;
        }
        // ==================================
        // Ignore whitespace on source stream
        // ==================================
        void ignoreWS()
        {
            while (more() && isWS())
            {
                next();
            }
        }
        // =================================================================
        // Is current string a match at the current source stream position ?
        // =================================================================
        bool match(const String &targetString)
        {
            long index = 0;
            while (more() && current() == targetString[index])
            {
                next();
                if (++index == (long)targetString.length())
                {
                    return (true);
                }
            }
            backup(index);
            return (false);
        }
        // ==================================
        // Get current source stream position
        // ==================================
        std::pair<long, long> getPosition() const
        {
            return (std::make_pair(m_lineNo, m_column));
        }
        // =============================================
        // Read bytes representing the current character
        // =============================================
        std::string current_to_bytes()
        {
            return (m_UTF8.to_bytes(current()));
        }
        // ==============================
        // Convert to byte string (UTF-8)
        // ==============================
        std::string to_bytes(const String &from)
        {
            return (m_UTF8.to_bytes(from));
        }
        // ==============================
        // Convert to byte string (UTF-8)
        // ==============================
        std::string to_bytes(const Char from)
        {
            return (m_UTF8.to_bytes(from));
        }
        // ================================
        // Convert from byte string (UTF-8)
        // ================================
        String from_bytes(const std::string &from)
        {
            return (m_UTF8.from_bytes(from));
        }

    protected:
        long m_lineNo = 1;
        long m_column = 1;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
        std::wstring_convert<std::codecvt_utf8_utf16<Char>, Char> m_UTF8;
    };
} // namespace XML_Lib