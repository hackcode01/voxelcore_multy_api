#include "path.hpp"

#include <stack>

using namespace io;

void Path::checkValid() const {
    if (m_colonPosition == std::string::npos) {
        throw std::runtime_error("path entry point is not specified: " + m_str);
    }
}

Path Path::parent() const {
    size_t length = m_str.length();
    while (length && m_str[length - 1] == '/') {
        --length;
    }
    size_t slashPosition = length;
    slashPosition = m_str.rfind('/', slashPosition - 1);
    if (length >= 2 && m_str.rfind("..") == length - 2) {
        return normalized().parent();
    }
    if (slashPosition == std::string::npos) {
        return m_colonPosition == std::string::npos
                   ? ""
                   : m_str.substr(0, m_colonPosition + 1);
    }
    while (slashPosition && m_str[slashPosition - 1] == '/') {
        slashPosition--;
    }
    return m_str.substr(0, slashPosition);
}

Path Path::normalized() const {
    io::Path path = pathPart();

    std::stack<io::Path> parts;
    int64_t position = 0;
    int64_t prevPosition = position - 1;
    while (position < path.m_str.length()) {
        position = path.m_str.find('/', position);

        if (position == std::string::npos) {
            parts.push(path.m_str.substr(prevPosition + 1));
            break;
        }

        if (position - prevPosition == 0) {
            prevPosition = position;
            position = prevPosition + 1;
            continue;
        }
        auto token = path.m_str.substr(prevPosition + 1, position - (prevPosition + 1));
        prevPosition = position;

        if (token == ".") {
            continue;
        } else if (token == "..") {
            if (parts.empty()) {
                throw AccessError("entry-point reached");
            }
            parts.pop();
            continue;
        }
        parts.push(std::move(token));
    }

    path = "";
    while (!parts.empty()) {
        const auto& token = parts.top();
        if (path.empty()) {
            path = token;
        } else {
            path = token / path;
        }
        parts.pop();
    }

    if (m_colonPosition != std::string::npos) {
        path = m_str.substr(0, m_colonPosition + 1) + path.string();
    }

    return path;
}
