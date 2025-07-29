#pragma once

#include <memory>

#include "io/fwd.hpp"

namespace audio {
    struct PCM;
    class PCMStream;
}

namespace wav {
    std::unique_ptr<audio::PCM> load_pcm(
        const io::Path& file, bool headerOnly
    );
    std::unique_ptr<audio::PCMStream> create_stream(
        const io::Path& file
    );
}
