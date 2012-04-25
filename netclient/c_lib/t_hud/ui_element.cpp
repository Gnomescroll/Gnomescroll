#include "ui_element.hpp"

#include <c_lib/physics/common.hpp>

#include <c_lib/t_hud/constants.hpp>

#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/toolbelt_hud.hpp>

namespace t_hud
{

/*
int InventoryUI::get_slot_at(int px, int py)
{
    px -= this->x + this->border;
    py -= this->y - this->border;

    float width  = xdim*slot_size + (xdim-1)*icon_spacing;
    float height = ydim*slot_size + (ydim-1)*icon_spacing;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (icon_spacing + slot_size);
    int yslot = py / (icon_spacing + slot_size);

    int slot = yslot * this->xdim + xslot;
    
    //printf("inventory: slot %d, %d\n", xslot, yslot);

    return slot;
}

InventoryUI* nanite_inventory = NULL;
InventoryUI* craft_bench_inventory = NULL;

void init_nanite_inventory_ui()
{
    assert(nanite_inventory == NULL);
    
}

void init_craft_bench_inventory_ui()
{
    assert(craft_bench_inventory == NULL);

}

void teardown_inventory_ui()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_inventory != NULL) delete nanite_inventory;
    if (craft_bench_inventory != NULL) delete craft_bench_inventory;
}

InventoryUI* get_inventory_hud_element(HudElementType type)
{
    switch (type)
    {
        case HUD_ELEMENT_AGENT_INVENTORY:
            return agent_inventory;
        case HUD_ELEMENT_AGENT_TOOLBELT:
            return agent_toolbelt;
        case HUD_ELEMENT_NANITE_INVENTORY:
            return nanite_inventory;
        case HUD_ELEMENT_CRAFTING_BENCH:
            return craft_bench_inventory;
            
        default:
            printf("WARNING: get_inventory_hud_element -- invalid type %d\n", type);
            assert(false);
            break;
    }
    return NULL;
}
*/

} // t_hud
