#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class StackableComponent: public Component
{
    public:
        int max;
        int size;
        
        bool merge_stack(StackableComponent* stack);
        
    StackableComponent()
    : Component(COMPONENT_STACKABLE, COMPONENT_INTERFACE_STACKABLE),
        max(1), size(1)
    {}
};


} // Components
