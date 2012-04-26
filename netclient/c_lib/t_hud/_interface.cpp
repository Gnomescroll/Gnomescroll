#include "_interface.hpp"

#include <entity/constants.hpp>
#include <t_hud/constants.hpp>
#include <t_hud/inventory_hud.hpp>
#include <t_hud/toolbelt_hud.hpp>
#include <t_hud/nanite_hud.hpp>

namespace t_hud
{

class AgentInventoryUI* agent_inventory;
class AgentToolbeltUI* agent_toolbelt;

// TODO -- TMP -- replace witha ctual types
class AgentNanite* nanite_inventory;
class AgentInventoryUI* craft_bench_inventory;

float mouse_x;
float mouse_y;

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

int active_slot = NULL_SLOT;
UIElement* active_inventory = NULL;

static UIElement* get_inventory_and_slot(int x, int y, int* slot)
{
    // track topmost clicked inventory
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

    mouse_x = x;
    mouse_y = y;
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

static void draw_grabbed_icon()
{
    if (active_slot == NULL_SLOT) return;
    if (active_inventory == NULL) return;
    InventorySlot* contents = Items::get_inventory_contents(active_inventory->inventory_id);
    if (contents == NULL) return;
    if (contents[active_slot].empty()) return;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_BLEND);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);
    
    const float w = 32;

    // center icon on mouse position
    const float x = mouse_x - (w / 2);
    const float y = _yresf - (mouse_y + (w / 2));
    
    int tex_id = contents[active_slot].sprite_index;

    //const float iw = 8.0f; // icon_width
    //const int iiw = 8; // integer icon width
    const float iw = 16.0f; // icon_width
    const int iiw = 16; // integer icon width
    
    const float tx_min = (1.0/iw)*(tex_id % iiw);
    const float ty_min = (1.0/iw)*(tex_id / iiw);
    const float tx_max = tx_min + 1.0/iw;
    const float ty_max = ty_min + 1.0/iw;

    glTexCoord2f( tx_min, ty_min );
    glVertex2f(x,y+w);

    glTexCoord2f( tx_max, ty_min );
    glVertex2f(x+w, y+w );
        
    glTexCoord2f( tx_max, ty_max );
    glVertex2f(x+w, y);

    glTexCoord2f( tx_min, ty_max );
    glVertex2f(x, y);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}


void draw_hud()
{
    if (!hud_enabled) return;

    agent_inventory->draw();
    agent_toolbelt->draw();
    //nanite_inventory->draw();
    //craft_bench_inventory->draw();

    draw_grabbed_icon();
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
    agent_inventory->init();

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->xoff = 500.0f;
    agent_toolbelt->yoff = 500.0f;
    agent_toolbelt->init();

    nanite_inventory = new AgentNanite;
    nanite_inventory->xoff = 0.0f;
    nanite_inventory->yoff = 0.0f;
    nanite_inventory->init();
}

void teardown()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_inventory != NULL) delete nanite_inventory;
    if (craft_bench_inventory != NULL) delete craft_bench_inventory;
}



}
