#pragma once

// inherit this into a component for tracking
class ComponentProperties
{
    public:
    int id;
    ObjectPolicyInterface* obj;
    void* list;
    
    ComponentProperties()
    : id(-1), obj(NULL), list(NULL)
    {}
};


