#include "item.hpp"

#if DC_SERVER
#include <item/server.hpp>
#endif

namespace Item
{

void Item::init(int item_type)
{
    this->type = item_type;
    this->group = get_item_group_for_type(item_type);
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    this->energy = attr->max_energy;
    this->durability = attr->max_durability;
}

}   // Item
