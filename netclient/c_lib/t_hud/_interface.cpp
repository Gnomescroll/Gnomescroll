#include "_interface.hpp"

#include <c_lib/entity/constants.hpp>
#include <c_lib/t_hud/constants.hpp>
#include <c_lib/t_hud/inventory_hud.hpp>
#include <c_lib/t_hud/toolbelt_hud.hpp>

namespace t_hud
{

class AgentInventoryUI* agent_inventory;
class AgentToolbeltUI* agent_toolbelt;

// TODO -- TMP -- replace witha ctual types
AgentInventoryUI* nanite_inventory;
AgentInventoryUI* craft_bench_inventory;


/*
    Init
*/
void draw_init()
{
    init_texture();
}

void draw_teardown()
{
    teardown_texture();
}
/*
    Input Handling
*/

static bool hud_enabled = false;
void enable_inventory_hud()
{
    hud_enabled = true;
}

void disable_inventory_hud()
{
    hud_enabled = false;
}

static int active_slot = NULL_SLOT;
static UIElement* active_inventory = NULL;

static UIElement* get_inventory_and_slot(int x, int y, int* slot)
{
    // track topmost clicked inventory
    //float closest = 100000.0f;
    UIElement* closest_inventory = NULL;
    int closest_slot = NULL_SLOT;

    // set up inventory array
    const int n_inventories = 4;
    UIElement* inventories[n_inventories] = {
        agent_inventory,
        agent_toolbelt,
        nanite_inventory,
        craft_bench_inventory,
    };

    // get topmost inventory click
    UIElement* inventory;
    int slot_tmp;
    for (int i=0; i<n_inventories; i++)
    {
        inventory = inventories[i];
        if (inventory == NULL) continue;
        slot_tmp = inventory->get_slot_at(x,y);
        if (slot_tmp == NULL_SLOT) continue;
        //if (inventory->z > closest) continue;
        //closest = inventory->z;
        closest_inventory = inventory;
        closest_slot = slot_tmp;
    }

    *slot = closest_slot;
    return closest_inventory;
}


void mouse_motion(int x, int y)
{
    //printf("t_item::mouse_motion x,y= %i %i \n", x,y);

    // if slot selected
    // draw that slot's icon at cursor

    
}

void left_mouse_down(int x, int y)
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;

    // set active slot/inventory
    active_inventory = get_inventory_and_slot(x,y, &active_slot);
    if (active_inventory == NULL) return;
    active_inventory->selected_slot = active_slot;
}

void left_mouse_up(int x, int y)
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;

    if (active_inventory == NULL) return;

    // detect click
    int slot;
    UIElement* inventory = get_inventory_and_slot(x,y, &slot);

    // decide swap event
    InventoryInputEventType event_type = INVENTORY_INPUT_EVENT_NONE;
    if (slot == NULL_SLOT || inventory == NULL)     // clicked outside inventory UIs
        event_type = INVENTORY_INPUT_EVENT_REMOVE;
    else
    {   // clicked in an inventory
        if (active_inventory == inventory) event_type = INVENTORY_INPUT_EVENT_SWAP_WITHIN;
        else event_type = INVENTORY_INPUT_EVENT_SWAP_BETWEEN;
    }
    
    inventory_input_event.type = event_type;
    inventory_input_event.inventory = active_inventory->inventory_id;
    inventory_input_event.slot = active_slot;

    if (inventory != NULL)
        inventory_input_event.inventory_b = inventory->inventory_id;
    if (slot != NULL_SLOT)
        inventory_input_event.slot_b = slot;

    // reset actives
    active_inventory->selected_slot = NULL_SLOT;
    active_inventory = NULL;
    active_slot = NULL_SLOT;
}

void right_mouse_down(int x, int y)
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;

    // lock slot
}

void right_mouse_up(int x, int y)
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;

    // if slot matches locked slot,
    // remove
}

void null_input_event()
{
    inventory_input_event.type = INVENTORY_INPUT_EVENT_NONE;
}

/*
    Drawing
*/
void draw_hud()
{
    if (!hud_enabled) return;

    agent_inventory->draw();
    agent_toolbelt->draw();

    //static ItemGrid g;
    //g.draw(300,300);
}


void network_inventory_assignment(ObjectType type, int id)
{
    switch (type)
    {
        case OBJECT_AGENT_INVENTORY:
            agent_inventory->inventory_id = id;
            break;
        case OBJECT_AGENT_TOOLBELT:
            agent_toolbelt->inventory_id = id;
            break;
        case OBJECT_NANITE_INVENTORY:
            nanite_inventory->inventory_id = id;
            break;
        case OBJECT_CRAFTING_BENCH:
            craft_bench_inventory->inventory_id = id;
            break;
        default:
            printf("WARNING -- network_inventory_assignment -- invalid inventory type %d\n", type);
            break;
    }
}


/* Main init/teardown */

void init()
{
    agent_inventory = new AgentInventoryUI;
    agent_inventory->xoff = 300.0f;
    agent_inventory->yoff = 100.0f;

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->xoff = 500.0f;
    agent_toolbelt->yoff = 500.0f;
}

void teardown()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_inventory != NULL) delete nanite_inventory;
    if (craft_bench_inventory != NULL) delete craft_bench_inventory;
}



}
