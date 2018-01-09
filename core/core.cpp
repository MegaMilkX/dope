#include <stdint.h>
#include <windows.h>
#include "screen_buffer.h"
#include <stdio.h>
#include <math.h>

#include "core_impl.h"

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT msg, DWORD_PTR dwInst, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
  if(msg == WOM_DONE)
  {
    
  }
}

int main()
{  
  Core core;
  core.Init();
  
  core.Run();
  
  core.Cleanup();
  
  return 0;
}