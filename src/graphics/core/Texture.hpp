#pragma once

#include "typedefs.hpp"
#include "maths/UVRegion.hpp"
#include "ImageData.hpp"

#include <memory>

class Texture {
protected:
    uint_t id;
    uint_t width;
    uint_t height;
public:
    Texture(uint_t id, uint_t width, uint_t height);
    Texture(const ubyte* data, uint_t width, uint_t height, ImageFormat format);
    virtual ~Texture();

    virtual void bind() const;
    virtual void unbind() const;
    void reload(const ubyte* data);

    void setNearestFilter();

    void reload(const ImageData& image);

    void setMipMapping(bool flag, bool pixelated);

    std::unique_ptr<ImageData> readData();
    uint_t getId() const;

    UVRegion getUVRegion() const {
        return UVRegion(0.0f, 0.0f, 1.0f, 1.0f);
    }

    uint_t getWidth() const {
        return width;
    }

    uint_t getHeight() const {
        return height;
    }

    static std::unique_ptr<Texture> from(const ImageData* image);
    static uint_t MAX_RESOLUTION;
};
