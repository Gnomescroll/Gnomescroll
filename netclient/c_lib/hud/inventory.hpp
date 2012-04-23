#pragma once

#include <c_lib/hud/constants.hpp>

namespace HudInventory
{

class InventoryRender
{
    public:
        bool inited;
        bool visible;
        float x,y;
        float z;
        
        //GLuint item_sheet_texture;
        //GLuint item_slot_texture;
        GLuint inventory_background_texture;
        float width,height;

        void set_position(float x, float y);
        void init();
        void draw();

        InventoryRender();
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
