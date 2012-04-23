#include "contents.hpp"

#if DC_SERVER
void InventoryProperties::load(int id, ObjectType type, int stack_size)
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
void InventoryProperties::load(int id, ObjectType type, int stack_size)
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

// TODO:: will not need this
void InventoryProperties::get_sprite_data(struct Draw::SpriteData* data)
{
    using Items::current_render_inventory;
    HudInventory::InventoryRender* render = Items::get_render_inventory();
    if (render == NULL) return;
    
    data->index = this->sprite_index;
    float x = render->x + (this->spacing/4);
    float y = render->y + render->height - 3*(this->spacing/4); // need to subtract our height
    data->x = x + this->spacing * (slot % current_render_inventory->width());
    data->y = y - this->spacing * (slot / current_render_inventory->width());
    data->z = render->z + 0.01;
}
#endif
