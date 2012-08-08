#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <item/config/item_drop.hpp>

namespace Components
{

class ItemDropComponent: public Component
{
    public:

    Item::ItemDrop drop;
    void drop_item();

    ItemDropComponent()
    : Component(COMPONENT_ITEM_DROP, COMPONENT_INTERFACE_ITEM_DROP)
    {}

};


}   // Components
