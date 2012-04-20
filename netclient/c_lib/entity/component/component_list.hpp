#pragma once

#include <c_lib/entity/component/component.hpp>

namespace Components
{

template <class Component, ComponentType TYPE, int SIZE>
class ComponentList
{
    protected:
        ComponentType type;
        //Component* components;  // TODO -- dont malloc everything
        int count;
        int max;
        Component** components;

    public:

        Component* subscribe()
        {   // return pointer to available component
            for (int i=0; i<SIZE; i++)
                if (this->components[i] == NULL)
                {
                    this->components[i] = new Component;
                    this->components[i]->id = i;
                    return this->components[i];
                }
            return NULL;
        }
        
        void unsubscribe(Component* component)
        {   // deallocate/release component in components
            this->components[component->id] = NULL;
            delete component;
        }
            
        void init()
        {
            //this->components = new components[SIZE];
            this->components = (Component**)calloc(SIZE, sizeof(Component*));
        }
    
    ComponentList<Component,TYPE,SIZE>()
    : type(TYPE), count(0), max(SIZE), components(NULL)
    {
        this->init();
    }
    
    ~ComponentList<Component,TYPE,SIZE>()
    { 
        if (this->components != NULL) delete this->components;
    }
};

template <class Component, ComponentType TYPE, int SIZE>
class CallableComponentList: public ComponentList<Component,TYPE,SIZE>
{
    public:
        void call()
        {
            for (int i=0; i<this->max; i++)
                if (this->components[i] != NULL)
                    this->components[i]->call();
        }
};

} // Components
