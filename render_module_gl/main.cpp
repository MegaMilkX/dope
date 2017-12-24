#include "../include/core.h"
#include <windows.h>
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb_image.h"
}

int tbpp = 0;
int twidth = 0;
int theight = 0;
unsigned char* texData = 0;

void RenderInit()
{
  stbi_set_flip_vertically_on_load(1);
  texData = stbi_load("test.png", &twidth, &theight, &tbpp, 4);
}

void RenderUpdate(ScreenBuffer* screen)
{
  POINT pt;
  GetCursorPos(&pt);
  
  unsigned x_offset = pt.x, y_offset = screen->height - pt.y;
    for(unsigned y = 0; (y + y_offset) < screen->height && y < theight; ++y)
    {
      for(unsigned x = 0; (x + x_offset) < screen->width && x < twidth; ++x)
      {
        screen->data[((y + y_offset) * screen->width + (x + x_offset)) * screen->bpp] = texData[(y * twidth + x) * tbpp + 2];
        screen->data[((y + y_offset) * screen->width + (x + x_offset)) * screen->bpp + 1] = texData[(y * twidth + x) * tbpp + 1];
        screen->data[((y + y_offset) * screen->width + (x + x_offset)) * screen->bpp + 2] = texData[(y * twidth + x) * tbpp];
        screen->data[((y + y_offset) * screen->width + (x + x_offset)) * screen->bpp + 3] = 255;
      }
    }
}

void RenderCleanup()
{
  stbi_image_free(texData);
}