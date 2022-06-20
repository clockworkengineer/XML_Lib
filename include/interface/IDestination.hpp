#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <codecvt>
#include <locale>
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // =======================================================================
    // Interface for writing destination stream during XML/DTD stringification
    // =======================================================================
    class IDestination
    {
    public:
        // ==================
        // IDestination Error
        // ==================
        struct Error : public std::exception
        {
            explicit Error(const std::string &description = "")
            {
                errorMessage = "IDestination Error: " + description;
            }
            [[nodiscard]] const char *what() const noexcept override
            {
                return (errorMessage.c_str());
            }
        private:
            std::string errorMessage;
        };
        // ========================
        // Constructors/destructors
        // ========================
        IDestination() = default;
        IDestination(const IDestination &other) = delete;
        IDestination &operator=(const IDestination &other) = delete;
        IDestination(IDestination &&other) = delete;
        IDestination &operator=(IDestination &&other) = delete;
        virtual ~IDestination() = default;
        // ========================
        // Add bytes to destination
        // ========================
        virtual void add(const std::string &bytes) = 0;
        virtual void add(ISource::Char c) = 0;
    protected:
        std::wstring_convert<std::codecvt_utf8_utf16<ISource::String::value_type>, ISource::String::value_type> m_UTF8;
    };
} // namespace XMLLib