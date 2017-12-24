#ifndef RENDER_MODULE_H
#define RENDER_MODULE_H

#include "screen_buffer.h"

#define DLLEXPORT __declspec(dllexport)

extern "C"{
  
DLLEXPORT void RenderInit();
DLLEXPORT void RenderUpdate(ScreenBuffer* screen);
DLLEXPORT void RenderCleanup();

}
#endif
