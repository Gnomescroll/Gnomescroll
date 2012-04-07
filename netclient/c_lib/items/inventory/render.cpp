#include "render.hpp"


int get_icon_spritesheet_id(Object_types type)
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            return 1;
        default:
            return 0;
    }
}

void draw_inventory_icon(Object_types type, int slot)
{   // lookup icon index from type, draw at slot position
}

void register_inventory_item_draw_list(InventoryProperties* property)
{
    #if DC_CLIENT
     // map id,type to
    int spritesheet = get_icon_spritesheet_id(property->type);
    if (spritesheet < 0) return;
    //ClientState::draw_lists[spritesheet]->register_object(property);
    #endif
}

void unregister_inventory_item_draw_list(InventoryProperties* property)
{
    #if DC_CLIENT
     //map id,type to
    int spritesheet = get_icon_spritesheet_id(property->type);
    if (spritesheet < 0) return;
    //ClientState::draw_lists[spritesheet]->unregister_object(property);
    #endif
}

void InventoryIconDrawList::draw()
{
    InventoryProperties* obj;
    for (int i=0; i<this->max; i++)
    {
        obj = this->objects[i];
        draw_inventory_icon(obj->type, obj->slot);
    }
}

