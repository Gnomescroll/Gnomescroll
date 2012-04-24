#pragma once

#include <c_lib/hud/constants.hpp>

namespace HudInventory
{

class InventoryRender
{
    public:
        bool visible;       // render state
        
        float x,y;
        float z;
        float w,h;          // screen dimensions of panel

        float border;       // border around entire panel
        float icon_spacing; // spacing between slot icons
        float icon_border;  // border around a slot icon

        float slot_size;    // pixel dimension
        int xdim,ydim;  // slot dimensions
        
        GLuint background_texture;

        void set_position(float x, float y);
        void init();
        void draw();

    InventoryRender()
    :   visible(false),
        x(0),y(0), z(-0.5f),
        w(0), h(0),
        border(16.0f), icon_spacing(8.0f), icon_border(2.0f),
        slot_size(32.0f),
        xdim(0), ydim(0),
        background_texture(0)
    {}
};

extern InventoryRender* agent_inventory;
extern InventoryRender* agent_toolbelt;
extern InventoryRender* nanite_inventory;
extern InventoryRender* craft_bench_inventory;

void draw_icon_mask(float x, float y, float w, float h, float depth);
void draw_selected_icon_mask();
void init();
void teardown();

InventoryRender* get_inventory_hud_element(HudElementType type);
void get_screen_inventory_row_col(HudInventory::InventoryRender* inventory, int x, int y, int* row, int* col);

}
