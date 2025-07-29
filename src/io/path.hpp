#ifndef __PATH_HPP__
#define __PATH_HPP__

#include <filesystem>
#include <stdexcept>
#include <string>

namespace io {
    using file_time_type_t = std::filesystem::file_time_type;

    /// @brief Access violation error
    class AccessError : public std::runtime_error {
    public:
        AccessError(const std::string& message) : std::runtime_error(message) {
        }
    };

    /// @brief std::filesystem::path project-specific alternative having
    /// `entry_point:path` scheme and solving std::filesystem::path problems:
    /// - implicit std::string conversions depending on compiler
    /// - unicode Path construction must be done with std::filesystem::u8path
    class Path {
    public:
        Path() = default;

        Path(std::string str) : m_str(std::move(str)) {
            m_colonPosition = this->m_str.find(':');

            size_t length = this->m_str.length();
            for (size_t i {0}; i < length; ++i) {
                if (this->m_str[i] == '\\') {
                    this->m_str[i] = '/';
                }
            }
        }

        Path(const char* str) : Path(std::string(str)) {}

        bool operator==(const std::string& other) const {
            return m_str == other;
        }

        bool operator<(const Path& other) const {
            return m_str < other.m_str;
        }

        bool operator==(const Path& other) const {
            return m_str == other.m_str;
        }

        bool operator==(const char* other) const {
            return m_str == other;
        }

        Path operator/(const char* child) const {
            if (m_str.empty() || m_str[m_str.length() - 1] == ':') {
                return m_str + std::string(child);
            }
            return m_str + "/" + std::string(child);
        }

        Path operator/(const std::string& child) const {
            if (m_str.empty() || m_str[m_str.length() - 1] == ':') {
                return m_str + child;
            }
            return m_str + "/" + child;
        }

        Path operator/(std::string_view child) const {
            if (m_str.empty() || m_str[m_str.length() - 1] == ':') {
                return m_str + std::string(child);
            }
            return m_str + "/" + std::string(child);
        }

        Path operator/(const Path& child) const {
            if (m_str.empty() || m_str[m_str.length() - 1] == ':') {
                return m_str + child.pathPart();
            }
            return m_str + "/" + child.pathPart();
        }

        std::string pathPart() const {
            if (m_colonPosition == std::string::npos) {
                return m_str;
            }
            return m_str.substr(m_colonPosition + 1);
        }

        std::string name() const {
            size_t slashPosition = m_str.rfind('/');
            if (slashPosition == std::string::npos) {
                return m_colonPosition == std::string::npos
                           ? m_str
                           : m_str.substr(m_colonPosition + 1);
            }
            return m_str.substr(slashPosition + 1);
        }

        std::string stem() const {
            return name().substr(0, name().rfind('.'));
        }

        /// @brief Get extension
        std::string extension() const {
            size_t slashPosition = m_str.rfind('/');
            size_t dotPosition = m_str.rfind('.');
            if (dotPosition == std::string::npos ||
                (slashPosition != std::string::npos &&
                 dotPosition < slashPosition)) {
                return "";
            }

            return m_str.substr(dotPosition);
        }

        /// @brief Get entry point
        std::string entryPoint() const {
            checkValid();
            return m_str.substr(0, m_colonPosition);
        }

        /// @brief Get parent path
        Path parent() const;

        Path normalized() const;

        std::string string() const {
            return m_str;
        }

        /// @brief Check if path is not initialized with 'entry_point:path'
        bool empty() const {
            return m_str.empty();
        }

        bool emptyOrInvalid() const {
            return m_str.empty() || m_colonPosition == std::string::npos;
        }
    private:
        /// @brief UTF-8 string contains entry_point:path or empty string
        std::string m_str {};
        /// @brief Precalculated position of colon character
        size_t m_colonPosition {std::string::npos};

        void checkValid() const;
    };

    class PathsGenerator {
    public:
        virtual ~PathsGenerator() = default;
        virtual bool next(Path& dst) = 0;
    };
}

#endif
