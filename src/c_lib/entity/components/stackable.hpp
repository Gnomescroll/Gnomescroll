#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class StackableComponent: public Component
{
    public:
        int max;
        int count;

        bool merge_stack(StackableComponent* stack);

    StackableComponent()
    : Component(COMPONENT_STACKABLE, COMPONENT_INTERFACE_STACKABLE),
        max(1), count(1)
    {}
};


} // Components
