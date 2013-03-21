#pragma once

#include <entity/component/component.hpp>

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
        IF_ASSERT(this->components == NULL) return NULL;
        for (int i=0; i<this->max; i++)
            if (this->components[i] == NULL)
            {
                this->components[i] = new Component;
                this->components[i]->id = i;
                return this->components[i];
            }
        GS_ASSERT(false);
        printf("No component found\n");
        return NULL;
    }

    void unsubscribe(Component* component)
    {   // deallocate/release component in components
        IF_ASSERT(component == NULL) return;
        GS_ASSERT(this->components != NULL);
        GS_ASSERT(this->components[component->id] != NULL);
        this->components[component->id] = NULL;
        delete component;
    }

    void init()
    {
        IF_ASSERT(this->components != NULL) return;
        IF_ASSERT(this->max <= 0) return;
        this->components = (Component**)calloc(this->max, sizeof(Component*));
    }

    ComponentList<Component,TYPE,SIZE>() :
        type(TYPE), count(0), max(SIZE), components(NULL)
    {
        this->init();
    }

    ~ComponentList<Component,TYPE,SIZE>()
    {
        if (this->components == NULL) return;
        for (int i=0; i<this->max; i++)
            if (this->components[i] != NULL)
                delete this->components[i];
        free(this->components);
    }
};

template <class Component, ComponentType TYPE, int SIZE>
class CallableComponentList: public ComponentList<Component,TYPE,SIZE>
{
    public:
    void call()
    {
        IF_ASSERT(this->components == NULL) return;
        for (int i=0; i<this->max; i++)
            if (this->components[i] != NULL)
                this->components[i]->call();
    }
};

} // Components
