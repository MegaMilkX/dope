#include "../include/core.h"

#include <iostream>

SceneObject* scene;

int GameInit(CoreInterface* coreInterface)
{
  scene = coreInterface->CreateScene();
  
  
  return 0;
}

void GameUpdate()
{
  std::cout << scene->GetComponent<int>() << std::endl;
}

void GameCleanup()
{
  
}