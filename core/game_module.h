#ifndef GAME_MODULE_H
#define GAME_MODULE_H

#include "core_interface.h"

#define DLLEXPORT __declspec(dllexport)

extern "C"{
  
DLLEXPORT int GameInit(CoreInterface* coreInterface);
DLLEXPORT void GameUpdate();
DLLEXPORT void GameCleanup();

}
#endif
