#pragma once

#if DC_CLIENT
# error Do not include this file in the client
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

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(ItemDropComponent);
        COPY(drop);
    }

    ItemDropComponent() :
        Component(COMPONENT_ItemDrop, COMPONENT_INTERFACE_ItemDrop)
    {
        this->drop.vx_func = &default_random_velocity;
        this->drop.vy_func = &default_random_velocity;
    }

};


}   // Components
