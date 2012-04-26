#include "slot.hpp"

#if DC_SERVER
void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    this->item_id = id;
    this->item_type = type;
    this->count = stack_size;
}

#endif

#if DC_CLIENT

#include <entity/objects/gemstones/constants.hpp>

// TODO -- move

int get_icon_sprite_index(ObjectType type)
{
    const int ERROR_SPRITE = 48;    // "no"
    switch (type)
    {
        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
            return Objects:: MALACHITE_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_RUBY:
            return Objects:: RUBY_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_TURQUOISE:
            return Objects:: TURQUOISE_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_SILVER:
            return Objects:: SILVER_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_AMETHYST:
            return Objects:: AMETHYST_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_JADE:
            return Objects:: JADE_GEMSTONE_SPRITE_INDEX;
        case OBJECT_GEMSTONE_ONYX:
            return Objects:: ONYX_GEMSTONE_SPRITE_INDEX;

        // block drops
        //case OBJECT_DIRT_BLOCK_DROP:
            //return ERROR_SPRITE;
            //break;
        //case OBJECT_STONE_BLOCK_DROP:
            //return GEMSTONE_MALACHITE_ICON_SPRITE;
            //break;
        //case OBJECT_SOFT_ROCK_BLOCK_DROP:
            //return GEMSTONE_MALACHITE_ICON_SPRITE;
            //break;
        //case OBJECT_MEDIUM_ROCK_BLOCK_DROP:
            //return GEMSTONE_MALACHITE_ICON_SPRITE;
            //break;
        //case OBJECT_HARD_ROCK_BLOCK_DROP:
            //return GEMSTONE_MALACHITE_ICON_SPRITE;
            //break;
        //case OBJECT_INFECTED_ROCK_BLOCK_DROP:
            //return GEMSTONE_MALACHITE_ICON_SPRITE;
            //break;

        default: return ERROR_SPRITE;
    }
    return ERROR_SPRITE;
}

void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    #if DC_CLIENT
    this->sprite_index = get_icon_sprite_index(type);
    #endif
    this->item_id = id;
    this->item_type = type;
    this->count = stack_size;
}
#endif
