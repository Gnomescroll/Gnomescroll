#pragma once

#include <item/properties.hpp>

namespace t_hud
{

class CraftingUI : public UIElement
{
    private:
        int get_grid_at(int px, int py);

    public:
          
    static const int cell_size = 37;
    static const int xdim = 6;    // grid cell size
    static const int ydim = 1;

    static const int input_xdim = 4;
    static const int input_ydim = 1;

    static const int input_slots = 4;
    static const int output_slots = 1;
    static const int input_output_gap = 1;

    // size of texture/render area
    static const float render_width = cell_size * 6;
    static const float render_height = cell_size * 1;

    static const float slot_size = 32;
    static const float cell_offset_x = 3;
    static const float cell_offset_y = 3;
    static const float cell_offset_x_right = 2;
    static const float cell_offset_y_bottom = 2;
    static const float input_offset_x = 0;
    static const float input_offset_y = 0;
    static const float output_offset_x = cell_size * (input_xdim + input_output_gap);
    static const float output_offset_y = 0;

    HudText::Text* stacks;

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

    bool in_craft_output_region(int px, int py);

    void init()
    {
        GS_ASSERT(this->stacks == NULL);

        int max = input_slots;
        this->stacks = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->stacks[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(STACK_COUNT_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }
    }

    CraftingUI()
    : stacks(NULL)
    {}

    ~CraftingUI()
    {
        if (this->stacks != NULL) delete[] this->stacks;
    }
};

int CraftingUI::get_grid_at(int px, int py)
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

int CraftingUI::get_slot_at(int px, int py)
{  
    int slot = this->get_grid_at(px,py);
    // filter out non-slots
    if (slot >= input_slots + input_output_gap + output_slots) return NULL_SLOT;
    if (slot >= input_slots && slot < input_slots + input_output_gap) return NULL_SLOT;
    // transform to output region slot
    if (slot >= input_slots + input_output_gap) slot -= input_slots + input_output_gap;
    return slot;
}

bool CraftingUI::in_craft_output_region(int px, int py)
{
    return (this->get_grid_at(px,py) >= input_slots + input_output_gap);
}

void CraftingUI::draw()
{
    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture( GL_TEXTURE_2D, CraftingTexture );

    glColor4ub(255, 255, 255, 255);

    float w = render_width;
    float h = render_height;

    float x = xoff;
    float y = yoff;

    float tx_min = 0.0;
    float ty_min = 0.0;
    float tx_max = render_width/512.0;
    float ty_max = render_height/512.0;

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

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    
    if (hover_slot != NULL_SLOT)
    {
        // shift to output region
        if (this->in_craft_output_region(mouse_x, mouse_y)) hover_slot += input_slots + input_output_gap;
        int w = slot_size;
        int xslot = hover_slot % this->xdim;
        int yslot = hover_slot / this->xdim;

        const float x = xoff + cell_size*xslot + cell_offset_x;
        const float y = yoff - (cell_size*yslot + cell_offset_y);
        
        glVertex2f(x,y);
        glVertex2f(x, y-w);
        glVertex2f(x+w, y-w);
        glVertex2f(x+w, y);
    }
    glEnd();

    if (this->container_id == NULL_CONTAINER) return;

    // get data for rendering items
    int* slot_types = ItemContainer::get_container_ui_types(this->container_id);
    int* slot_stacks = ItemContainer::get_container_ui_stacks(this->container_id);
    if (slot_types == NULL) return;
    GS_ASSERT(slot_stacks != NULL);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, TextureSheetLoader::ItemSheetTexture);

    glBegin(GL_QUADS);

    //draw input items
    for (int xslot=0; xslot<input_xdim; xslot++)
    for (int yslot=0; yslot<input_ydim; yslot++)
    {
        int slot = input_xdim*yslot + xslot;
        int item_type = slot_types[slot];
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + input_offset_x + cell_offset_x + cell_size*xslot;
        const float y = yoff - (input_offset_y + cell_offset_y + cell_size*yslot);

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

    //draw output items
    bool available;
    int item_type = Item::get_selected_craft_recipe_type(this->container_id, 0, &available);
    if (item_type != NULL_ITEM_TYPE)
    {
        int tex_id = Item::get_sprite_index_for_type(item_type);

        // switch to greyscale texture if unavailable
        if (!available) glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::GreyScaleItemTexture);

        x = xoff + output_offset_x + cell_offset_x;// + cell_size*xslot;
        y = yoff - (output_offset_y + cell_offset_y);// + cell_size*yslot);

        w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        tx_min = (1.0/iw)*(tex_id % iiw);
        ty_min = (1.0/iw)*(tex_id / iiw);
        tx_max = tx_min + 1.0/iw;
        ty_max = ty_min + 1.0/iw;

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

    // draw stacks
    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    HudText::Text* text;
    for (int xslot=0; xslot<input_xdim; xslot++)
    for (int yslot=0; yslot<input_ydim; yslot++)
    {
        // the nanite store slots in dat are indexed from 0
        // it is not aware of the implementation detail we have for food
        const int slot = input_xdim*yslot + xslot;

        int stack = slot_stacks[slot];
        if (stack <= 1) continue;

        GS_ASSERT(count_digits(stack) < STACK_COUNT_MAX_LENGTH);

        text = &this->stacks[slot];
        text->update_formatted_string(1, stack);

        const float x = xoff + input_offset_x + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        const float y = yoff - (input_offset_y + cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());

        text->set_position(x,y);
        text->draw();
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    //u_dot(xoff,yoff);
    glColor4ub(255, 255, 255, 255);

}

}   // t_hud
