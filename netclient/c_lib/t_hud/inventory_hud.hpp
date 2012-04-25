#pragma once

#include <c_lib/t_hud/texture.hpp>

#include <c_lib/SDL/SDL_functions.h>

namespace t_hud
{


class InventoryUI
{
    public:
        HudElementType type;
        bool visible;       // render state
        
        float x,y;
        float z;
        float w,h;          // screen dimensions of panel

        float border;       // border around entire panel
        float icon_spacing; // spacing between slot icons
        float icon_border;  // border around a slot icon

        float slot_size;    // pixel dimension
        int xdim,ydim;  // slot dimensions

        int active_slot;
        
        GLuint background_texture;

        void set_position(float x, float y);
        void init();
        void draw();

        int get_slot_at(int px, int py);

    InventoryUI()
    :   type(HUD_ELEMENT_NONE),
        visible(false),
        x(0),y(0), z(-0.5f),
        w(0), h(0),
        border(16.0f), icon_spacing(8.0f), icon_border(2.0f),
        slot_size(32.0f),
        xdim(0), ydim(0),
        background_texture(0)
    {}
};



extern InventoryUI* agent_inventory;
extern InventoryUI* agent_toolbelt;
extern InventoryUI* nanite_inventory;
extern InventoryUI* craft_bench_inventory;

void init_agent_inventory_ui()
{
    assert(agent_inventory == NULL);
    agent_inventory = new InventoryUI;

    agent_inventory->x = 300.0f;
    agent_inventory->y = 300.0f;

    // these will be received from network
    agent_inventory->xdim = 6;
    agent_inventory->ydim = 3;
}

void init_agent_toolbelt_ui()
{
    assert(agent_toolbelt == NULL);
    agent_inventory = new InventoryUI;

    agent_inventory->x = 300.0f;
    agent_inventory->y = 300.0f;

    // these will be received from network
    agent_inventory->xdim = 6;
    agent_inventory->ydim = 3;
}

void init_nanite_inventory_ui()
{}

void init_craft_bench_inventory_ui()
{}

void teardown_inventory_ui()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (nanite_inventory != NULL) delete nanite_inventory;
    if (craft_bench_inventory != NULL) delete craft_bench_inventory;
}

}
