#include "contents.hpp"


#if DC_CLIENT
void register_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->register_object(property);
}

void unregister_inventory_item_draw_list(InventoryProperties* property)
{
    Draw::draw_lists->get(Draw::ITEM_DRAW_LIST)->unregister_object(property);
}
#endif


/* Inventory Properties */

#if DC_CLIENT
void InventoryProperties::get_sprite_data(struct Draw::SpriteData* data)
{
    data->index = this->sprite_index;
    const float spacing = 64.0f;
    data->x = inventory->screen.x + spacing * slot;
    data->y = inventory->screen.y;
    data->z = inventory->screen.z;
}
#endif

/* Inventory Contents */

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
        msg.slot = i;
        msg.sendToClient(client_id);
    }
}

