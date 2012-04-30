#include "_interface.hpp"

#include <entity/constants.hpp>
#include <t_hud/constants.hpp>
#include <t_hud/inventory_hud.hpp>
#include <t_hud/toolbelt_hud.hpp>
#include <t_hud/nanite_hud.hpp>

namespace t_hud
{

class AgentInventoryUI* agent_container;
class AgentToolbeltUI* agent_toolbelt;
// TODO -- TMP -- replace witha ctual types
class AgentNaniteUI* nanite_container;
class AgentInventoryUI* craft_bench_container;

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
        default:
            assert(false);
            return;
    }
}


/*
    Input Handling
*/

static bool hud_enabled = false;
float mouse_x;
float mouse_y;

void enable_container_hud()
{
    hud_enabled = true;
}

void disable_container_hud()
{
    hud_enabled = false;
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
        craft_bench_container,
    };

    // get topmost container click
    UIElement* container;
    int slot_tmp;
    for (int i=0; i<n_inventories; i++)
    {
        container = inventories[i];
        if (container == NULL) continue;
        slot_tmp = container->get_slot_at(x,y);
        if (slot_tmp == NULL_SLOT) continue;
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
    return event;
}

static ContainerInputEvent null_event = {
    -1,         // null container id
    NULL_SLOT   // null slot
};

ContainerInputEvent left_mouse_down(int x, int y)
{
    return null_event;
}

ContainerInputEvent left_mouse_up(int x, int y)
{
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent right_mouse_down(int x, int y)
{
    return null_event;
}

ContainerInputEvent right_mouse_up(int x, int y)
{
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent mouse_motion(int x, int y)
{
    mouse_x = x;
    mouse_y = y;
    return null_event;
}

/*
    Drawing
*/

static HudText::Text* grabbed_icon_stack_text = NULL;


static void draw_grabbed_icon()
{
    if (Item::player_hand_type_ui == NULL_ITEM_TYPE) return;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_BLEND);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);
    
    const float w = 32;

    // center icon on mouse position
    float x = mouse_x - (w / 2);
    float y = _yresf - (mouse_y + (w / 2));
    
    int tex_id = Item::get_sprite_index_for_type(Item::player_hand_type_ui);

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
    //if (Item::player_hand_stack_ui <= 1) return;
    assert(Item::player_hand_stack_ui < 100);   // string only fits 99

    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    // calc posuition
    x = x + (w/2) + font_size;
    y = y + (w/2) - font_size;
    grabbed_icon_stack_text->update_formatted_string(1, Item::player_hand_stack_ui);
    grabbed_icon_stack_text->set_position(x,y);
    grabbed_icon_stack_text->draw();

    HudFont::reset_default();
    HudFont::end_font_draw();
}


void draw_hud()
{
    if (!hud_enabled) return;

    agent_container->draw();
    agent_toolbelt->draw();
    //nanite_container->draw();
    //craft_bench_container->draw();

    draw_grabbed_icon();
}

/* Main init/teardown */

/*
    Init
*/


void init()
{
    agent_container = new AgentInventoryUI;
    agent_container->xoff = 300.0f;
    agent_container->yoff = 100.0f;
    agent_container->init();

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->xoff = 500.0f;
    agent_toolbelt->yoff = 500.0f;
    agent_toolbelt->init();

    nanite_container = new AgentNaniteUI;
    nanite_container->xoff = 0.0f;
    nanite_container->yoff = 0.0f;
    nanite_container->init();

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
    if (craft_bench_container != NULL) delete craft_bench_container;

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
