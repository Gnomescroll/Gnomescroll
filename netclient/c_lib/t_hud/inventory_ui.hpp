#pragma once

#include <c_lib/t_hud/constants.hpp>

namespace t_hud
{


class UIelement
{
        public:

        bool visible;
        float xoff;
        float yoff;

        int inventory_id;

        void set_position(float x, float y);

        virtual void init() = 0;
        virtual void draw() = 0;
        virtual bool get_slot_at(int px, int py, int* slotx, int* sloty) = 0;
};

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

        // this data is mirrored in the Items::Inventory
        int inventory_id;
        int xdim,ydim;  // slot dimensions

        int selected_slot;
        
        void set_position(float x, float y);
        void init();

        int get_slot_at(int px, int py);

    InventoryUI()
    :   type(HUD_ELEMENT_NONE),
        visible(false),
        x(0),y(0), z(-0.5f),
        w(0), h(0),
        border(16.0f), icon_spacing(8.0f), icon_border(2.0f),
        slot_size(32.0f),
        inventory_id(-1),
        xdim(0), ydim(0),
        selected_slot(NULL_SLOT)
    {}
};

extern InventoryUI* nanite_inventory;
extern InventoryUI* craft_bench_inventory;

void init_nanite_inventory_ui();
void init_craft_bench_inventory_ui();

void teardown_inventory_ui();

InventoryUI* get_inventory_hud_element(HudElementType type);

} // t_hud
