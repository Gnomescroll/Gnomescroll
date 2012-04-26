#include "client.hpp"

#if DC_CLIENT

#include <c_lib/items/inventory/_interface.hpp>

/* Inventory */

int get_icon_spritesheet_id(ObjectType type)
{
    const int ERROR_SPRITE = 0;
    switch (type)
    {
        //case OBJECT_REFILL:
        //case OBJECT_BLOCK_DROP:
        //case OBJECT_GEMSTONE:
            //return ItemDrops::get_object_type_sprite_index(type);

        default: return ERROR_SPRITE;
    }
}
#endif
