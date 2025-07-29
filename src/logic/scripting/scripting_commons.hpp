#pragma once

#include <string>

#include "io/fwd.hpp"

namespace scripting {
    void load_script(const io::Path& name, bool throwable);

    [[nodiscard]] int load_script(
        int env,
        const std::string& type,
        const io::Path& file,
        const std::string& fileName
    );
}
