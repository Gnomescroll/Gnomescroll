#pragma once

#include <c_lib/entity/component/component.hpp>

namespace Components
{

template <class Component, ComponentType TYPE, int SIZE>
class ComponentList
{
    protected:
        ComponentType type;
        //Component* pool;  // TODO -- dont malloc everything
        int ctr;
        Component** pool;

    public:

        Component* subscribe()
        {   // return pointer to available component
            for (int i=0; i<SIZE; i++)
                if (this->pool[i] == NULL)
                {
                    this->pool[i] = new Component;
                    this->pool[i]->id = i;
                    return this->pool[i];
                }
            return NULL;
        }
        
        void unsubscribe(Component* component)
        {   // deallocate/release component in pool
            this->pool[component->id] = NULL;
            delete component;
        }
            
        void init()
        {
            //this->pool = new pool[SIZE];
            this->pool = (Component**)calloc(SIZE, sizeof(Component*));
        }
    
    ComponentList<Component,TYPE,SIZE>()
    : type(TYPE), ctr(0), pool(NULL)
    {
        this->init();
    }
    
    ~ComponentList<Component,TYPE,SIZE>()
    { 
        if (this->pool != NULL) delete this->pool;
    }
};

} // Components
