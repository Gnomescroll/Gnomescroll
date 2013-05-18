#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <item/config/item_drop.hpp>

namespace Components
{

float default_random_velocity()
{
    static const float mom = 2.0f;
    return (randf()-0.5f)*mom;
}

class ItemDropComponent: public Component
{
    public:

    Item::ItemDrop drop;
    void drop_item();

    ItemDropComponent()
    : Component(COMPONENT_ITEM_DROP, COMPONENT_INTERFACE_ITEM_DROP)
    {
        this->drop.vx_func = &default_random_velocity;
        this->drop.vy_func = &default_random_velocity;
    }

};


}   // Components
