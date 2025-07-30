#pragma once

#include "typedefs.hpp"
#include "commons.hpp"
#include "ImageData.hpp"

class GBuffer : public Bindable {
public:
    GBuffer(uint_t width, uint_t height);
    ~GBuffer() override;

    void bind() override;
    void bindSSAO() const;
    void unbind() override;

    void bindBuffers() const;
    void bindSSAOBuffer() const;

    void bindDepthBuffer(int drawFbo);

    void resize(uint_t width, uint_t height);

    uint_t getWidth() const;
    uint_t getHeight() const;

    std::unique_ptr<ImageData> toImage() const;
private:
    uint_t width;
    uint_t height;

    uint_t fbo;
    uint_t colorBuffer = 0;
    uint_t positionsBuffer = 0;
    uint_t normalsBuffer = 0;
    uint_t emissionBuffer = 0;
    uint_t depthBuffer = 0;
    uint_t ssaoFbo = 0;
    uint_t ssaoBuffer = 0;

    void createColorBuffer();
    void createPositionsBuffer();
    void createNormalsBuffer();
    void createEmissionBuffer();
    void createDepthBuffer();
    void createSSAOBuffer();
};
