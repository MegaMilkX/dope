#ifndef SCENE_OBJECT_IMPL_H
#define SCENE_OBJECT_IMPL_H

#include "core_interface.h"
#include "scene_object.h"

#include <vector>
#include <map>

class SceneObjectImpl : public SceneObject
{
public:
    SceneObjectImpl(CoreInterface* ci)
    : coreInterface(ci), parent(0) {}
    SceneObjectImpl(CoreInterface* ci, SceneObject* parent)
    : coreInterface(ci), parent(parent) {}
    
    virtual SceneObject*        GetRoot()
    {
        if(parent)
          return parent->GetRoot();
        else
          return this;
    }
    virtual bool                IsRoot()
    {
        if(!parent)
          return true;
        else
          return false;
    }
    virtual SceneObject*        CreateObject(const char* name)
    {
        SceneObject* o = new SceneObjectImpl(coreInterface, this);
        objects.push_back(o);
        return o;
    }
    virtual SceneObject*        FindObject(const char* name)
    {
        SceneObject* o = 0;
        for (auto so : objects)
        {
            if (so->Name() == name)
            {
                o = so;
                break;
            }
            else if (o = so->FindObject(name))
            {
                break;
            }
        }

        return o;
    }
    virtual Component*          GetComponent(const char* type)
    {
        Component* c = FindComponent(type);
        if (!c)
        {
            Component* c = coreInterface->CreateComponent(this, type);           
            return c;
        }
        else
            return c;
    }
    virtual Component*          FindComponent(const char* type)
    {
        if (components.empty())
            return 0;
        auto it = components.find(type);
        if (it == components.end())
            return 0;
        else
            return it->second;
    }
    virtual void                Name(const char* name)
    {
        this->name = name;
    }
    virtual const char*         Name()
    {
        return name.c_str();
    }
private:
    CoreInterface* coreInterface;
    
    std::string name;
    SceneObject* parent;
    std::vector<SceneObject*> objects;
    std::map<std::string, Component*> components;
};

#endif
