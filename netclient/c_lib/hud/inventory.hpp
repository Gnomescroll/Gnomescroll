#pragma once

namespace HudInventory
{

class Inventory
{
    public:
        bool inited;
        float x,y;

        int item_sheet_texture;
        int item_slot_texture;
        int inventory_background_texture;

        void set_position(float x, float y);
        void init();
        void draw();

        Inventory();
};

extern Inventory inventory;

void init();

}
