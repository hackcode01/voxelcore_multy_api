#include "commons.hpp"
#include <sstream>
#include <stdexcept>

#include "util/stringutil.hpp"

parsing_error::parsing_error(
    const std::string& message,
    std::string_view filename,
    std::string_view source,
    uint_t pos,
    uint_t line,
    uint_t linestart
)
    : std::runtime_error(message),
      filename(filename),
      pos(pos),
      line(line),
      linestart(linestart) {
    size_t end = source.find("\n", linestart);
    if (end == std::string::npos) {
        end = source.length();
    }
    this->source = source.substr(linestart, end - linestart);
}

parsing_error::parsing_error(
    const std::string& message,
    std::string&& filename,
    std::string&& source,
    uint_t pos,
    uint_t line,
    uint_t linestart
)
    : std::runtime_error(message),
      filename(std::move(filename)),
      pos(pos),
      line(line),
      linestart(linestart) {
    size_t end = source.find("\n", linestart);
    if (end == std::string::npos) {
        end = source.length();
    }
    this->source = source.substr(linestart, end - linestart);
}

std::string parsing_error::errorLog() const {
    std::stringstream ss;
    uint_t linepos = pos - linestart;
    ss << "parsing error in file '" << filename;
    ss << "' at " << (line + 1) << ":" << linepos << ": " << this->what()
       << "\n";
    ss << source << "\n";
    for (uint_t i = 0; i < linepos; i++) {
        ss << " ";
    }
    ss << "^";
    return ss.str();
}
