#pragma once

#include <item/properties.hpp>

namespace t_hud
{

class CraftingUI : public UIElement
{
    public:


          
//    static const float inc1 = 6; // spacing between slot icons
//    static const float inc2 = 2;  // border around a slot icon
//    static const float item_size = 32;    // pixel dimension

    static const float cell_size = 37;
    static const int xdim = 4;    // grid cell size
    static const int ydim = 2;

    // size of texture/render area
    static const float render_width = 37*xdim;
    static const float render_height = 37*ydim;

    //HudText::Text* prices;
    //HudText::Text* stacks;

    void draw();

    int width()
    {
        return render_width;
    }

    int height()
    {
        return render_height;
    }

    int get_slot_at(int px, int py);

    bool point_inside(int px, int py)
    {
        return (this->get_slot_at(px,py) != NULL_SLOT);
    }

    bool in_nanite_region(int px, int py);

    void init()
    {
/*
        int max = (xdim * ydim) - 1;    // last slot is reserved

        assert(this->prices == NULL);
        this->prices = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->prices[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(ITEM_PRICE_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }

        // stacks only for food/coins
        assert(this->stacks == NULL);
        this->stacks = new HudText::Text[max];
        for (int i=0; i<2; i++)
        {
            HudText::Text* t = &this->stacks[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(STACK_COUNT_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }
*/
    }

    CraftingUI()
    {}

    ~CraftingUI()
    {
    }
};


int CraftingUI::get_slot_at(int px, int py)
{  
    //pixels from upper left
    px -= xoff;
    py -= _yresf - yoff;

    if (px < 0 || px > render_width)  return NULL_SLOT;
    if (py < 0 || py > render_height) return NULL_SLOT;

    int xslot = px / cell_size;
    int yslot = py / cell_size;
    int slot = xslot + yslot * xdim;

    return slot;
}

void CraftingUI::draw()
{
    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture( GL_TEXTURE_2D, NaniteTexture );

    glColor4ub(255, 255, 255, 255);

    const float w = render_width;
    const float h = render_height;

    float x = xoff;
    float y = yoff;

    const float tx_min = 0.0;
    const float ty_min = 0.0;
    const float tx_max = render_width/512.0;
    const float ty_max = render_height/512.0;

    //draw background
    glBegin(GL_QUADS);

    glTexCoord2f( tx_min, ty_min );
    glVertex2f(x, y);

    glTexCoord2f( tx_min, ty_max );
    glVertex2f(x,y-h);

    glTexCoord2f( tx_max, ty_max );
    glVertex2f(x+w, y-h );

    glTexCoord2f( tx_max, ty_min );
    glVertex2f(x+w, y);

    glEnd();


    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);
}
/*
    glDisable(GL_TEXTURE_2D);
    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    if (hover_slot != 0 && hover_slot != NULL_SLOT)
    {
        hover_slot -= 1;
        int w = slot_size;
        int xslot = hover_slot % this->xdim;
        int yslot = hover_slot / this->xdim;
        
        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot;
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);

        glVertex2f(x,y);
        glVertex2f(x, y-w);
        glVertex2f(x+w, y-w);
        glVertex2f(x+w, y);
    }
    glEnd();


    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);

    //draw store items
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        if (xslot == xdim-1 && yslot == ydim-1) continue;    // this is the last slot, put money here

        int item_type, cost;
        Item::get_nanite_store_item(level, xslot, yslot, &item_type, &cost);
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot;
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(tex_id % iiw);
        const float ty_min = (1.0/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glTexCoord2f( tx_min, ty_min );
        glVertex2f(x, y);

        glTexCoord2f( tx_min, ty_max );
        glVertex2f(x,y-w);

        glTexCoord2f( tx_max, ty_max );
        glVertex2f(x+w, y-w );

        glTexCoord2f( tx_max, ty_min );
        glVertex2f(x+w, y);
    }
    glEnd();


    // draw food
    Item::ItemContainerNaniteUI* container = (Item::ItemContainerNaniteUI*)Item::get_container_ui(this->container_id);
    if (container == NULL) return;

    int food_item_type = container->get_food_type();
    if (food_item_type != NULL_ITEM_TYPE)
    {
        int food_sprite_id = Item::get_sprite_index_for_type(food_item_type);
        float x = xoff + nanite_offset_x + nanite_width - slot_size;
        float y = yoff - (nanite_offset_y + nanite_height - slot_size);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(food_sprite_id % iiw);
        const float ty_min = (1.0/iw)*(food_sprite_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glBegin(GL_QUADS);
        glTexCoord2f( tx_min, ty_min );
        glVertex2f(x, y);

        glTexCoord2f( tx_min, ty_max );
        glVertex2f(x,y-w);

        glTexCoord2f( tx_max, ty_max );
        glVertex2f(x+w, y-w );

        glTexCoord2f( tx_max, ty_min );
        glVertex2f(x+w, y);
        glEnd();
    }
    
    // draw coins
    int coin_item_type = container->get_coin_type();
    if (coin_item_type != NULL_ITEM_TYPE)
    {
        int xslot = xdim-1;
        int yslot = ydim-1;
        
        int coin_sprite_id = Item::get_sprite_index_for_type(coin_item_type);
        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot;
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(coin_sprite_id % iiw);
        const float ty_min = (1.0/iw)*(coin_sprite_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glBegin(GL_QUADS);
        glTexCoord2f( tx_min, ty_min );
        glVertex2f(x, y);

        glTexCoord2f( tx_min, ty_max );
        glVertex2f(x,y-w);

        glTexCoord2f( tx_max, ty_max );
        glVertex2f(x+w, y-w );

        glTexCoord2f( tx_max, ty_min );
        glVertex2f(x+w, y);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    //draw text for item cost in upper right
    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    HudText::Text* text;
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        // the nanite store slots in dat are indexed from 0
        // it is not aware of the implementation detail we have for food
        const int slot = yslot*xdim + xslot;
        if (slot == xdim*ydim-1) continue;  // skip last slot, reserved

        int item_type, cost;
        Item::get_nanite_store_item(level, xslot, yslot, &item_type, &cost);
        if (item_type == NULL_ITEM_TYPE) continue;

        assert(count_digits(cost) < ITEM_PRICE_MAX_LENGTH);

        text = &this->prices[slot];
        text->update_formatted_string(1, cost);

        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot + slot_size - text->get_width();
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);
        
        text->set_position(x,y);
        text->draw();
    }

    // draw food stack
    HudText::Text* food_stack = &this->stacks[0];

    int food_stack_size = container->get_food_stack();
    assert(count_digits(food_stack_size) < STACK_COUNT_MAX_LENGTH);
    if (food_stack_size > 1)
    {
        food_stack->update_formatted_string(1, food_stack_size);
        const float x = xoff + nanite_offset_x + nanite_width - food_stack->get_width();
        const float y = yoff - (nanite_offset_y + nanite_height - food_stack->get_height());
        food_stack->set_position(x,y);
        food_stack->draw();
    }
    
    // draw coin stack
    HudText::Text* coin_stack = &this->stacks[0];

    int coin_stack_size = container->get_coin_stack();
    assert(count_digits(coin_stack_size) < STACK_COUNT_MAX_LENGTH);
    if (coin_stack_size > 1)
    {
        int xslot = xdim-1;
        int yslot = ydim-1;
        coin_stack->update_formatted_string(1, coin_stack_size);
        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot + slot_size - coin_stack->get_width();
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot + slot_size - coin_stack->get_height());
        coin_stack->set_position(x,y);
        coin_stack->draw();
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    //u_dot(xoff,yoff);
    glColor4ub(255, 255, 255, 255);
    */

}