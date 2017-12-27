#ifndef AUDIO_MODULE_H
#define AUDIO_MODULE_H

#define DLLEXPORT __declspec(dllexport)

extern "C"{
 
DLLEXPORT void AudioInit();
DLLEXPORT void AudioUpdate(void*, int, float);
DLLEXPORT void AudioCleanup(); 
  
}

#endif
