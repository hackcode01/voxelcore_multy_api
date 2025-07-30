#pragma once

#include "typedefs.hpp"

class ShadowMap {
public:
    ShadowMap(int resolution);
    ~ShadowMap();

    void bind();
    void unbind();
    uint_t getDepthMap() const;
    int getResolution() const;
private:
    uint_t fbo;
    uint_t depthMap; 
    int resolution;
};
