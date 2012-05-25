#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

#include <item/common/constant.hpp>

namespace Components
{

class ItemDropComponent: public Component
{
    public:

        int item_type;
        float probability;
        int max_amount;

        void drop_item();

    ItemDropComponent()
    : Component(COMPONENT_ITEM_DROP, COMPONENT_INTERFACE_ITEM_DROP),
    item_type(NULL_ITEM_TYPE), probability(1.0f), max_amount(1)
    {}
};

}   // Components
