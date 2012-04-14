#pragma once

namespace HudInventory
{

class Inventory
{
    public:
        bool inited;
        float x,y;
        float z;
        
        //GLuint item_sheet_texture;
        //GLuint item_slot_texture;
        GLuint inventory_background_texture;
        float width,height;

        void set_position(float x, float y);
        void init();
        void draw();

        Inventory();
};

extern Inventory* inventory;

void draw_icon_mask(float x, float y, float w, float h, float depth);
void init();
void teardown();

}
