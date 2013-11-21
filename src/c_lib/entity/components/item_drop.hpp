/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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
    private:
        bool owns_drop;

    public:
        Item::ItemDrop* drop;

    void drop_item();

    virtual void load_settings_from(const Component* component)
    {
        BEGIN_COPY(ItemDropComponent);
        COPY(drop);
    }

    virtual void use_as_default()
    {
        this->owns_drop = true;
        this->drop = new Item::ItemDrop;
        this->drop->vx_func = &default_random_velocity;
        this->drop->vy_func = &default_random_velocity;
    }

    virtual void on_destroy()
    {
        this->drop_item();
    }

    virtual ~ItemDropComponent()
    {
        if (this->owns_drop)
            delete this->drop;
        this->drop = NULL;
    }

    ItemDropComponent() :
        Component(COMPONENT_ItemDrop, COMPONENT_INTERFACE_ItemDrop),
        owns_drop(false)
    {
    }
};


}   // Components
