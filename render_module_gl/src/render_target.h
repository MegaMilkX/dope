#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "texture2d.h"
#include "framebuffer.h"

class RenderScene;
class RenderTarget : public IRenderTarget
{
public:
    RenderTarget(unsigned width, unsigned height)
        : frameBuffer(width, height)
    {
        frameBuffer.SetTexture(0, &color);
    }
    ~RenderTarget()
    {

    }

    void Resize(unsigned width, unsigned height)
    {
        frameBuffer.Resize(width, height);
    }

    void Bind()
    {
        frameBuffer.Bind();
    }

    FrameBuffer* GetFrameBuffer() { return &frameBuffer; }
    Texture2D* GetColorBuffer() { return &color; }

    FrameBuffer frameBuffer;
    Texture2D color;
    Viewpoint viewpoint;
};

#endif
