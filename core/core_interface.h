#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "scene_object.h"

class CoreInterface
{
public:
    virtual SceneObject* CreateScene() = 0;
    virtual void DestroyScene(SceneObject*) = 0;
    virtual Component* CreateComponent(SceneObject* so, const char* type) = 0;
};

#endif
