/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <entity/component/component.hpp>
#include <entity/components.hpp>

namespace Components
{

class ComponentList
{
    public:
        ComponentType type;
        ComponentInterfaceType interface;
        size_t count;
        size_t max;
        size_t call_tick;
        size_t call_rate;
        bool autocall;

    bool is_valid_id(ComponentID id)
    {
        return (id >= 0 && id < ComponentID(this->max));
    }

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
        this->interface = get_interface_for_component(type);
    }
};

template <class COMPONENT, ComponentType TYPE>
class ComponentListChild: public ComponentList
{
    private:
        size_t _index;

    public:
        COMPONENT* components;

    COMPONENT* subscribe()
    {   // return pointer to available component
        IF_ASSERT(this->components == NULL) return NULL;
        for (size_t i=0; i<this->max; i++)
        {
            size_t index = (i + this->_index) % this->max;
            if (this->components[index].id == NULL_COMPONENT)
            {
                new (&this->components[index]) COMPONENT;
                this->components[index].id = ComponentID(index);
                this->count++;
                this->_index = (index + 1) % this->max;
                return &this->components[index];
            }
        }
        GS_ASSERT(false);
        return NULL;
    }

    void unsubscribe(Component* component)
    {   // deallocate/release component in components
        IF_ASSERT(component == NULL) return;
        ComponentID id = component->id;
        IF_ASSERT(!this->is_valid_id(id)) return;
        IF_ASSERT(this->components[id].id == NULL_COMPONENT) return;
        this->count--;
        this->components[id].~COMPONENT();
        this->components[id].id = NULL_COMPONENT;
    }

    void call()
    {
        if ((++this->call_tick) % this->call_rate != 0)
            return;
        IF_ASSERT(this->components == NULL) return;
        for (size_t i=0, j=0; i<this->max && j < this->count; i++)
            if (this->components[i].id != NULL_COMPONENT)
            {
                j++;
                this->components[i].call();
            }
    }

    void init(size_t max)
    {
        IF_ASSERT(this->components != NULL) return;
        IF_ASSERT(max <= 0) return;
        this->max = max;
        this->components = (COMPONENT*)calloc(this->max, sizeof(*this->components));
        for (size_t i=0; i<this->max; i++)
            this->components[i].id = NULL_COMPONENT;
    }

    ComponentListChild<COMPONENT, TYPE>() :
        ComponentList(TYPE),
        _index(0), components(NULL)
    {
    }

    ~ComponentListChild<COMPONENT, TYPE>()
    {
        if (this->components == NULL) return;
        for (size_t i=0; i<this->max; i++)
            if (this->components[i].id != NULL_COMPONENT)
                this->components[i].~COMPONENT();
        free(this->components);
    }
};

} // Components
