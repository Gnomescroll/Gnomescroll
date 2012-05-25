#include "_interface.hpp"

#include <entity/constants.hpp>
#include <t_hud/constants.hpp>
#include <t_hud/container_hud.hpp>
#include <t_hud/toolbelt_hud.hpp>
#include <t_hud/nanite_hud.hpp>
#include <t_hud/storage_block.hpp>

namespace t_hud
{

class AgentContainerUI* agent_container;
class AgentToolbeltUI* agent_toolbelt;
class AgentNaniteUI* nanite_container;
class CraftingUI* crafting_container;
class StorageBlockUI* storage_block;

void set_container_id(ItemContainerType container_type, int container_id)
{
    switch (container_type)
    {
        case AGENT_CONTAINER:
            agent_container->container_id = container_id;
            break;
        case AGENT_TOOLBELT:
            agent_toolbelt->container_id = container_id;
            break;
        case AGENT_NANITE:
            nanite_container->container_id = container_id;
            break;
            
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            crafting_container->container_id = container_id;
            break;

        // TODO -- cryofreezer UI widget
        //case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            //cryofreezer->container_id = container_id;
            //break;

        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            storage_block->container_id = container_id;
            break;
            
        default:
            GS_ASSERT(false);
            return;
    }
}

void close_container(int container_id)
{
    //if (agent_container != NULL && agent_container->container_id == container_id) agent_container->container_id = NULL_CONTAINER;
    //if (agent_toolbelt != NULL && agent_toolbelt->container_id == container_id) agent_toolbelt->container_id = NULL_CONTAINER;
    //if (nanite_container != NULL && nanite_container->container_id == container_id) nanite_container->container_id = NULL_CONTAINER;
    
    // unset ids for variable container UIs
    if (crafting_container != NULL && crafting_container->container_id == container_id) crafting_container->container_id = NULL_CONTAINER;
    if (storage_block != NULL && storage_block->container_id == container_id) storage_block->container_id = NULL_CONTAINER;
}

/*
    Input Handling
*/

static bool agent_container_enabled = false;
static bool container_block_enabled = false;
static bool storage_block_enabled = false;
float mouse_x = -1;
float mouse_y = -1;

void enable_agent_container_hud()
{
    GS_ASSERT(!container_block_enabled);
    GS_ASSERT(!storage_block_enabled);
    agent_container_enabled = true;
}

void disable_agent_container_hud()
{
    // reset mouse state
    mouse_x = -1;
    mouse_y = -1;
    agent_container_enabled = false;
}

void enable_crafting_container_hud()
{
    GS_ASSERT(!agent_container_enabled);
    GS_ASSERT(!storage_block_enabled);
    container_block_enabled = true;
}

void disable_crafting_container_hud()
{
    // reset mouse state
    mouse_x = -1;
    mouse_y = -1;
    container_block_enabled = false;
}

void enable_storage_block_hud()
{
    GS_ASSERT(!agent_container_enabled);
    GS_ASSERT(!container_block_enabled);
    storage_block_enabled = true;
}

void disable_storage_block_hud()
{
    // reset mouse state
    mouse_x = -1;
    mouse_y = -1;
    storage_block_enabled = false;
}

static UIElement* get_container_and_slot(int x, int y, int* slot)
{
    // track topmost clicked container
    UIElement* closest_container = NULL;
    int closest_slot = NULL_SLOT;

    // set up container array
    const int n_inventories = 4;
    UIElement* inventories[n_inventories] = {
        agent_container,
        agent_toolbelt,
        nanite_container,
        crafting_container,
    };

    // get topmost container click
    UIElement* container;
    int slot_tmp;
    for (int i=0; i<n_inventories; i++)
    {
        container = inventories[i];
        if (container == NULL) continue;
        if (container->container_id == NULL_CONTAINER) continue;
        if (!container->point_inside(x,y)) continue;
        slot_tmp = container->get_slot_at(x,y);
        closest_container = container;
        closest_slot = slot_tmp;
    }

    *slot = closest_slot;
    return closest_container;
}


static ContainerInputEvent get_container_hud_ui_event(int x, int y)
{
    // detect click
    int slot;
    UIElement* container = get_container_and_slot(x,y, &slot);

    int container_id = NULL_CONTAINER;
    if (container != NULL) container_id = container->container_id;
    
    ContainerInputEvent event;
    event.container_id = container_id;
    event.slot = slot;
    event.nanite = (container != NULL
                 && container->type == UI_ELEMENT_NANITE_CONTAINER
                 && ((AgentNaniteUI*)container)->in_nanite_region(x,y));
    event.craft_output = (container != NULL
                        && container->type == UI_ELEMENT_CRAFTING_CONTAINER
                        && ((CraftingUI*)container)->in_craft_output_region(x,y));

    return event;
}

static const ContainerInputEvent NULL_EVENT =
{
    NULL_CONTAINER,         // null container id
    NULL_SLOT,   // null slot
    false,  // nanite click
    false,  // craft output
};

ContainerInputEvent left_mouse_down(int x, int y)
{
    return NULL_EVENT;
}

ContainerInputEvent left_mouse_up(int x, int y)
{
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent right_mouse_down(int x, int y)
{
    return NULL_EVENT;
}

ContainerInputEvent right_mouse_up(int x, int y)
{
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent mouse_motion(int x, int y)
{
    mouse_x = x;
    mouse_y = y;
    return NULL_EVENT;
}

void set_mouse_position(int x, int y)
{
    mouse_x = x;
    mouse_y = y;
}

ContainerInputEvent scroll_up()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot -= 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;
    if (agent_toolbelt->selected_slot < 0) agent_toolbelt->selected_slot += agent_toolbelt->xdim;
    
    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
    event.nanite = false;
    return event;
}

ContainerInputEvent scroll_down()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot += 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;
    
    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
    event.nanite = false;
    return event;
}

ContainerInputEvent select_slot(int numkey)
{
    if (agent_toolbelt == NULL) return NULL_EVENT;

    if (numkey == 0) numkey = 10;
    int slot = numkey-1;
    if (slot < 0 || slot >= agent_toolbelt->xdim) return NULL_EVENT;
    agent_toolbelt->selected_slot = slot;
    
    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = slot;
    event.nanite = false;
    return event;
}

/*
    Drawing
*/

static HudText::Text* grabbed_icon_stack_text = NULL;


static void draw_grabbed_icon()
{
    if (ItemContainer::player_hand_type_ui == NULL_ITEM_TYPE) return;

    const float w = 32;

    // center icon on mouse position
    float x = mouse_x - (w / 2);
    float y = _yresf - (mouse_y + (w / 2));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

    // render durability
    int durability = ItemContainer::player_hand_durability_ui;
    if (durability != NULL_DURABILITY)
    {
        int max_durability = Item::get_max_durability(ItemContainer::player_hand_type_ui);
        float ratio = ((float)durability)/((float)max_durability);
        if (ratio >= 0.75)
            glColor4ub(7, 247, 0, 128);    // green
        else if (ratio >= 0.5)
            glColor4ub(243, 247, 0, 128);  // yellow
        else if (ratio >= 0.25)
            glColor4ub(247, 159, 0, 128);  // orange
        else if (ratio >= 0.05)
            glColor4ub(247, 71, 0, 128);    // red-orange
        else
            glColor4ub(247, 14, 0, 128);   // red

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }
    glEnd();

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_BLEND);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::ItemSheetTexture);

    glBegin(GL_QUADS);
        
    int tex_id = Item::get_sprite_index_for_type(ItemContainer::player_hand_type_ui);

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

    // Draw stack numbers
    if (grabbed_icon_stack_text == NULL) return;
    if (ItemContainer::player_hand_stack_ui <= 1) return;
    GS_ASSERT(count_digits(ItemContainer::player_hand_stack_ui) < STACK_COUNT_MAX_LENGTH);   // string only fits 99

    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    // calc posuition
    x = x + (w/2) + font_size;
    y = y + (w/2) - font_size;
    grabbed_icon_stack_text->update_formatted_string(1, ItemContainer::player_hand_stack_ui);
    grabbed_icon_stack_text->set_position(x,y);
    grabbed_icon_stack_text->draw();

    HudFont::reset_default();
    HudFont::end_font_draw();
    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}


void draw_hud()
{
    agent_toolbelt->draw();

    if (!agent_container_enabled && !container_block_enabled && !storage_block_enabled) return;

    agent_container->draw();
    nanite_container->draw();
    if (container_block_enabled) crafting_container->draw();
    if (storage_block_enabled) storage_block->draw();

    draw_grabbed_icon();

}

/* Main init/teardown */

/*
    Init
*/


void init()
{
    agent_container = new AgentContainerUI;
    agent_container->type = UI_ELEMENT_AGENT_CONTAINER;
    agent_container->xoff = (_xresf - agent_container->width())/2 + 1;  // +1 because the width is odd with odd valued inc1 and even valued xdim
    agent_container->yoff = _yresf/2 - (agent_container->height())/2;
    agent_container->init();

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->type = UI_ELEMENT_AGENT_TOOLBELT;
    agent_toolbelt->xoff = (_xresf - agent_toolbelt->width())/2;
    agent_toolbelt->yoff = _yresf - (agent_toolbelt->height());
    agent_toolbelt->init();

    nanite_container = new AgentNaniteUI;
    nanite_container->type = UI_ELEMENT_NANITE_CONTAINER;
    nanite_container->xoff = (_xresf - nanite_container->width())/2;
    nanite_container->yoff = 150.0 + (_yresf + nanite_container->height())/2;
    nanite_container->init();

    crafting_container = new CraftingUI;
    crafting_container->type = UI_ELEMENT_CRAFTING_CONTAINER;
    crafting_container->xoff = (_xresf - crafting_container->width())/2 + 1;
    crafting_container->yoff = -150.0 + (_yresf + crafting_container->height())/2;
    crafting_container->init();

    storage_block = new StorageBlockUI;
    storage_block->type = UI_ELEMENT_STORAGE_BLOCK;
    storage_block->xoff = (_xresf - storage_block->width())/2 + 1;
    storage_block->yoff = -150.0 + (_yresf + storage_block->height())/2;
    storage_block->init();

    grabbed_icon_stack_text = new HudText::Text;
    grabbed_icon_stack_text->set_format((char*) "%d");
    grabbed_icon_stack_text->set_format_extra_length(STACK_COUNT_MAX_LENGTH + 1 - 2);
    grabbed_icon_stack_text->set_color(255,255,255,255);
    grabbed_icon_stack_text->set_depth(-0.1f);
}

void teardown()
{
    if (agent_container != NULL) delete agent_container;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_container != NULL) delete nanite_container;
    if (crafting_container != NULL) delete crafting_container;
    if (storage_block != NULL) delete storage_block;

    if (grabbed_icon_stack_text != NULL) delete grabbed_icon_stack_text;
}

#if DC_CLIENT

void draw_init()
{
    init_texture();
}

void draw_teardown()
{
    teardown_texture();
}

#endif

}
