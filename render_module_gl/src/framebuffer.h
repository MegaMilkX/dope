#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <iostream>
#include "texture2d.h"

class FrameBuffer
{
public:
    FrameBuffer(unsigned width, unsigned height)
        : width(width), height(height)
    {
        glGenFramebuffers(1, &framebuffer);
        Bind();

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "FrameBuffer creation failed!" << std::endl;
        }
    }

    ~FrameBuffer()
    {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteRenderbuffers(1, &depthBuffer);
    }

    void Resize(unsigned width, unsigned height)
    {
        // TODO: 
        throw std::exception("FrameBuffer::Resize() not implemented");
    }

    void SetTexture(int layer, Texture2D* tex)
    {
        tex->Resize(width, height);

        Bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + layer, tex->glIndex(), 0);

        textures.push_back(tex);
        GLenum* drawBuffers = new GLenum[textures.size()];
        for (unsigned i = 0; i < textures.size(); ++i)
            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(textures.size(), drawBuffers);
        delete[] drawBuffers;
    }

    void Bind()
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    }
private:
    unsigned width, height;

    GLuint framebuffer;
    GLuint depthBuffer;

    std::vector<Texture2D*> textures;
};

#endif
