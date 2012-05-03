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
    assert(attr != NULL);
    this->energy = attr->max_energy;
    this->durability = attr->max_durability;
    //if (item_type == 8)
    //{
        printf("energy is %d\n", attr->max_energy);
        printf("durability is %d\n", attr->max_durability);
    //}
}

#if DC_SERVER
Item::~Item()
{
    broadcast_item_destroy(this->id);
}
#endif

}
