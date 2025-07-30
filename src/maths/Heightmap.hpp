#pragma once

#include <vector>
#include <string>
#include <optional>

#include "typedefs.hpp"
#include "maths/Heightmap.hpp"
#include "util/EnumMetadata.hpp"

enum class InterpolationType {
    NEAREST,
    LINEAR,
    CUBIC,
};

VC_ENUM_METADATA(InterpolationType)
    {"nearest", InterpolationType::NEAREST},
    {"linear", InterpolationType::LINEAR},
    {"cubic", InterpolationType::CUBIC},
VC_ENUM_END

class Heightmap {
    uint_t width, height;
    std::vector<float> buffer;
public:
    Heightmap(uint_t width, uint_t height)
        : width(width), height(height) {
        buffer.resize(width*height);
    }

    Heightmap(uint_t width, uint_t height, std::vector<float> buffer) 
    : width(width), height(height), buffer(std::move(buffer)) {}

    ~Heightmap() = default;

    void resize(uint_t width, uint_t height, InterpolationType interpolation);

    void crop(uint_t srcX, uint_t srcY, uint_t dstWidth, uint_t dstHeight);

    void clamp();

    uint_t getWidth() const {
        return width;
    }

    uint_t getHeight() const {
        return height;
    }

    float get(uint_t x, uint_t y) {
        return buffer.at(y * width + x);
    }

    float getUnchecked(uint_t x, uint_t y) {
        return buffer[y * width + x];
    }

    float* getValues() {
        return buffer.data();
    }

    const float* getValues() const {
        return buffer.data();
    }
};
