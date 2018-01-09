#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "typeindex.h"

class Component
{
public:
    virtual ~Component() {}
};

class SceneObject
{
public:
    virtual SceneObject*        GetRoot() = 0;
    virtual bool                IsRoot() = 0;
    virtual SceneObject*        CreateObject(const char* name) = 0;
    virtual SceneObject*        FindObject(const char* name) = 0;
    virtual Component*          GetComponent(const char* type) = 0;
    virtual Component*          FindComponent(const char* type) = 0;
    virtual void                Name(const char* name) = 0;
    virtual const char*         Name() = 0;

    SceneObject*                CreateObject() { return CreateObject(""); }
    
    template<typename T>
    T* GetComponent()
    {
        return (T*)GetComponent(type_name<T>());
    }

    template<typename T>
    T* FindComponent()
    {
        return (T*)FindComponent(type_name<T>());
    }
};

#endif
