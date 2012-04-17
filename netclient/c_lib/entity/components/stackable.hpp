#pragma once

#include <c_lib/entity/constants.hpp>

namespace Components
{

class StackableComponent: public Component
{
    public:
        int max;
        int size;
        
    bool merge_stack(StackableComponent* stack)
    {   // returns true if stack size changes
        int room = this->max - this->size;
        if (room <= 0) return false;
        int available = stack->size;
        if (available <= 0) return false;
        if (room < available) available = room;
        this->size += available;
        stack->size -= available;
        return true;
    }
        
    StackableComponent()
    : Component(COMPONENT_STACKABLE, COMPONENT_INTERFACE_STACKABLE),
    type(COMPONENT_STACKABLE), max(10), size(1)
    {}
};


} // Components
