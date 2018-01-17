#include "../include/core.h"
#include <windows.h>
#include <math.h>
#include <dsound.h>

extern "C"{
#include "stb_vorbis.h"
}

int channels = 2;
int sampleRate = 44100;
short* decoded;
int len;
int cursor = 0;

void AudioInit()
{
  len = stb_vorbis_decode_filename("test.ogg", &channels, &sampleRate, &decoded);
}

void AudioUpdate(void* data, int sampleSize, int bytesPassed)
{/*
  char* buffer = (char*)data;
  for (DWORD t = 0; t < sampleSize; ++t)
  {
    float cur = t/(float)sampleSize;
    buffer[t] = tanf(cur * (2300 * (1.0f - cur))) * sinf(cur * (2000 * cur)) * cosf(cur * 1000) * cosf(cur * 500) * cur * 255;
		//buffer[t] = static_cast<char>((((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6)) & 0xFF);
  }
  */
  cursor += bytesPassed;
  for(unsigned i = 0; i < sampleSize; i++)
  {
    //char smpl[2];
    //memcpy(smpl, decoded[i], 2);
    ((short*)data)[i] = decoded[i + cursor];
  }
  //memcpy((char*)data, (char*)decoded + cursor, sampleSize);
  
}

void AudioCleanup()
{
  
}