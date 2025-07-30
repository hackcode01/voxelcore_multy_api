#pragma once

#include "Texture.hpp"

/// @brief Cubemap texture
class Cubemap : public Texture {
public:
    Cubemap(uint_t width, uint_t height, ImageFormat format);

    virtual void bind() const override;
    virtual void unbind() const override;
};
