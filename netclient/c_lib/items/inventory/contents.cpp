#include "contents.hpp"

#if DC_SERVER
void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    this->item_id = id;
    this->item_type = type;
    this->stack.count = stack_size;
    this->stack.max = get_max_stack_size(type);
    //printf("Loaded inventory item %d,%d\n", id,type);
}

void InventoryContents::sendToClient(int inventory_id, int client_id)
{
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i].item_id == EMPTY_SLOT)
            continue;
        add_item_to_inventory_StoC msg;
        msg.inventory_id = inventory_id;
        msg.id = this->objects[i].item_id;
        msg.type = this->objects[i].item_type;
        msg.stack = this->objects[i].stack.count;
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}
#endif

#if DC_CLIENT
void InventorySlot::load(int id, ObjectType type, int stack_size)
{
    if (type != this->item_type)
    {
        //this->sprite_index = get_icon_spritesheet_id(type);
        // MAJO TODO -- HACK
        // hud icon spritesheet is 8x8 but rendered particle spritesheet is 16x16 -- but using same indexes
        // (should be using same sheet)
        //if (this->sprite_index >= 8)
            //this->sprite_index -= 8 * this->sprite_index/16;
    }
    this->item_id = id;
    this->item_type = type;
    this->stack.count = stack_size;
    this->stack.max = get_max_stack_size(type);
    //printf("Loaded inventory item %d,%d\n", this->item_id, this->item_type);
}
#endif
