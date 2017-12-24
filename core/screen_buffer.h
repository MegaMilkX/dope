#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

struct ScreenBuffer
{
  unsigned char* data;
  int width;
  int height;
  int bpp;
};

#endif
