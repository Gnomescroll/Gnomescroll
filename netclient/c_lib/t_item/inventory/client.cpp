#include "client.hpp"

#if DC_CLIENT

#include <c_lib/hud/inventory.hpp>
#include <c_lib/items/inventory/interface.hpp>

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

void Inventory::get_selected_icon_render_data(Draw::SpriteData* data)
{
    if (!this->selected()) return;

    InventoryProperties icon = this->contents.objects[0];

    HudInventory::InventoryRender* render = Items::get_render_inventory();
    if (render == NULL) return;
    float x = render->x;// + (icon.spacing/4);
    float y = render->y + render->height - icon.spacing; // need to subtract our height
    data->x = x + icon.spacing * (this->selected_slot % this->width());
    data->y = y - icon.spacing * (this->selected_slot / this->width());
    data->z = render->z + 0.01 + 0.01;
    data->w = icon.spacing;
    data->h = icon.spacing;
}


Inventory::Inventory(int id)
: BaseInventory(id),
selected_slot(-1), hud(HUD_ELEMENT_NONE)
{
}

void attach_inventory_to_owner(Inventory* inventory)
{
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    if (a == NULL) return;

    switch (inventory->type)
    {
        case OBJECT_AGENT_INVENTORY:
            if (a->status.inventory != NULL)
                printf("WARNING: reassigned agent inventory\n");
            a->status.inventory = inventory;
            break;
        case OBJECT_AGENT_TOOLBELT:
            if (a->status.toolbelt != NULL)
                printf("WARNING: reassigned agent toobelt\n");
            a->status.toolbelt = inventory;
            break;
        default:
            printf("WARNING:: attach_inventory_to_owner() -- unhandled inventory type %d\n", inventory->type);
            break;
    }
}

void attach_inventory_to_interface(Inventory* inventory)
{
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    if (a == NULL) return;
    if (a != ClientState::playerAgent_state.you) return;
    
    switch (inventory->type)
    {
        case OBJECT_AGENT_INVENTORY:
            Items::agent_inventory = inventory;
            break;
        case OBJECT_AGENT_TOOLBELT:
            Items::agent_toolbelt = inventory;
            break;
        case OBJECT_NANITE_INVENTORY:
            Items::nanite_inventory = inventory;
            break;
        case OBJECT_CRAFTING_BENCH:
            Items::craft_bench_inventory = inventory;
            break;
        default:
            printf("WARNING:: attach_inventory_to_interface() -- unhandled inventory type %d\n", inventory->type);
            break;
    }
}

#endif
