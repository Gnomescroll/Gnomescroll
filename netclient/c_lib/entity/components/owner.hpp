#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class OwnerComponent: public Component
{
    public:
        int owner;

        int get_owner() { return this->owner; }
        void set_owner(int owner) { this->owner = owner; }

        #if DC_SERVER
        void revoke();
        #endif

    OwnerComponent()
    : Component(COMPONENT_OWNER, COMPONENT_INTERFACE_OWNER),
    owner(NO_AGENT)
    {}
};

} // Objects
