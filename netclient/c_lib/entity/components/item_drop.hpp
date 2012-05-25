#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class ItemDropComponent: public Component
{
    public:

    ItemDropComponent()
    : Component(COMPONENT_ITEM_DROP, COMPONENT_INTERFACE_ITEM_DROP)
    {}
};

}   // Components
