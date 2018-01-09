#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "gl/glextutil.h"

class Texture2D
{
public:
    Texture2D()
    {
        glGenTextures(1, &texture);
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &texture);
    }

    GLuint glIndex() { return texture; }

    void Resize(unsigned width, unsigned height)
    {
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    void Upload(unsigned char* data, unsigned width, unsigned height, unsigned bpp)
    {
        if (bpp < 1 || bpp > 4)
            return;

        GLenum fmt;
        if (bpp == 1)
            fmt = GL_RED;
        else if (bpp == 2)
            fmt = GL_RG;
        else if (bpp == 3)
            fmt = GL_RGB;
        else if (bpp == 4)
            fmt = GL_RGBA;

        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    }

    void Bind(int layer = 0)
    {
        glActiveTexture(GL_TEXTURE0 + layer);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
private:
    GLuint texture;
};

#endif
