#include "Cubemap.hpp"
#include "gl_util.hpp"

#include <GL/glew.h>

Cubemap::Cubemap(uint_t width, uint_t height, ImageFormat imageFormat) 
  : Texture(0, width, height) 
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    uint_t format = gl::to_glenum(imageFormat);
    for (uint_t face = 0; face < 6; face++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 
            0, 
            format, 
            width, 
            height, 
            0, 
            format, 
            GL_UNSIGNED_BYTE, 
            NULL
        );
    }
}

void Cubemap::bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Cubemap::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
