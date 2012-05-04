#include "_interface.hpp"

#include <entity/constants.hpp>
#include <t_hud/constants.hpp>
#include <t_hud/container_hud.hpp>
#include <t_hud/toolbelt_hud.hpp>
#include <t_hud/nanite_hud.hpp>

namespace t_hud
{

class AgentContainerUI* agent_container;
class AgentToolbeltUI* agent_toolbelt;
// TODO -- TMP -- replace witha ctual types
class AgentNaniteUI* nanite_container;
class AgentContainerUI* craft_bench_container;

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
float mouse_x = -1;
float mouse_y = -1;

void enable_container_hud()
{
    hud_enabled = true;
}

void disable_container_hud()
{
    // reset mouse state
    mouse_x = -1;
    mouse_y = -1;
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

static const ContainerInputEvent NULL_EVENT = {
    NULL_CONTAINER,         // null container id
    NULL_SLOT   // null slot
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

ContainerInputEvent scroll_up()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot += 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;
    
    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
    return event;
}

ContainerInputEvent scroll_down()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot -= 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;
    if (agent_toolbelt->selected_slot < 0) agent_toolbelt->selected_slot += agent_toolbelt->xdim;
    
    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
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
    return event;
}

/*
    Drawing
*/

static HudText::Text* grabbed_icon_stack_text = NULL;


static void draw_grabbed_icon()
{
    if (Item::player_hand_type_ui == NULL_ITEM_TYPE) return;

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
    int durability = Item::player_hand_durability_ui;
    if (durability != NULL_DURABILITY)
    {
        int max_durability = Item::get_max_durability(Item::player_hand_type_ui);
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
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);
        
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
    if (Item::player_hand_stack_ui <= 1) return;
    assert(count_digits(Item::player_hand_stack_ui) < STACK_COUNT_MAX_LENGTH);   // string only fits 99

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
    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);

    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);


    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}


void draw_hud()
{
    agent_toolbelt->draw();

    if (!hud_enabled) return;

    agent_container->draw();
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
    agent_container = new AgentContainerUI;
    agent_container->xoff = 300.0f;
    agent_container->yoff = 100.0f;
    agent_container->init();

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->xoff = (_xresf - agent_toolbelt->width()) / 2;
    //agent_toolbelt->yoff = _yresf - (agent_toolbelt->border + agent_toolbelt->inc2);
    agent_toolbelt->yoff = _yresf - (agent_toolbelt->height());
    //printf("toolbelt height = %d\n", agent_toolbelt->height());
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
