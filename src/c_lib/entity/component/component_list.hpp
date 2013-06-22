#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class ComponentList
{
    public:
        ComponentType type;
        size_t count;
        size_t max;
        size_t call_tick;
        size_t call_rate;
        bool autocall;

    void set_autocall(size_t rate=1)
    {
        IF_ASSERT(rate <= 0) rate = 1;
        GS_ASSERT(!this->autocall);
        this->autocall = true;
        this->call_rate = rate;
    }

    virtual Component* subscribe() = 0;
    virtual void unsubscribe(Component* component) = 0;
    virtual void call() = 0;
    virtual void init(size_t max) = 0;

    virtual ~ComponentList() {}

    ComponentList(ComponentType type) :
        type(type), count(0), max(0), call_tick(0), call_rate(1),
        autocall(false)
    {
    }
};

template <class COMPONENT, ComponentType TYPE>
class ComponentListChild: public ComponentList
{
    public:
        COMPONENT** components;

    COMPONENT* subscribe()
    {   // return pointer to available component
        IF_ASSERT(this->components == NULL) return NULL;
        for (size_t i=0; i<this->max; i++)
            if (this->components[i] == NULL)
            {
                this->components[i] = new COMPONENT;
                this->components[i]->id = i;
                this->count++;
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
        this->count--;
        this->components[component->id] = NULL;
        delete component;
    }

    void call()
    {
        if ((++this->call_tick) % this->call_rate != 0)
            return;
        IF_ASSERT(this->components == NULL) return;
        for (size_t i=0, j=0; i<this->max && j < this->count; i++)
            if (this->components[i] != NULL)
            {
                j++;
                this->components[i]->call();
            }
    }

    void init(size_t max)
    {
        IF_ASSERT(this->components != NULL) return;
        IF_ASSERT(max <= 0) return;
        this->max = max;
        this->components = (COMPONENT**)calloc(this->max, sizeof(*this->components));
    }

    ComponentListChild<COMPONENT, TYPE>() :
        ComponentList(TYPE),
        components(NULL)
    {
    }

    ~ComponentListChild<COMPONENT, TYPE>()
    {
        if (this->components == NULL) return;
        for (size_t i=0; i<this->max; i++)
            if (this->components[i] != NULL)
                delete this->components[i];
        free(this->components);
    }
};

} // Components
