#pragma once

#include <item/properties.hpp>

namespace HudContainer
{

class CraftingUI : public UIElement
{
    private:
        int get_grid_at(int px, int py);

    public:

        static const int cell_size = 37;

        static const int input_xdim = 4;
        static const int input_ydim = 1;

        static const int output_xdim = 1;
        static const int output_ydim = 3;

        static const int input_slots = input_xdim * input_ydim;
        static const int output_slots = output_xdim * output_ydim;
        static const int input_output_gap = 1;

        static const int xdim = input_xdim + input_output_gap + output_xdim;    // grid cell size
        static const int ydim = (input_ydim > output_ydim) ? input_ydim : output_ydim;

        // size of render area (texture + clickable area (will render highlights there still)
        static const float render_width;
        static const float render_height;

        // texture area
        static const float texture_width;
        static const float texture_height;

        static const float slot_size;
        static const float cell_offset_x;
        static const float cell_offset_y;
        static const float cell_offset_x_right;
        static const float cell_offset_y_bottom;
        static const float input_offset_x;
        static const float input_offset_y;
        static const float output_offset_x;
        static const float output_offset_y;

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

    bool in_craft_input_region(int px, int py);
    bool in_craft_output_region(int px, int py);

    void init()
    {
        this->init_item_labels(this->input_slots + this->output_slots);
    }

    void set_container_type(ItemContainerType container_type)
    {
        this->container_type = container_type;
    }

    CraftingUI()
    {}

    virtual ~CraftingUI()
    {
    }
};

// size of render area (texture + clickable area (will render highlights there still)
const float CraftingUI::render_width = CraftingUI::cell_size * CraftingUI::xdim;
const float CraftingUI::render_height = CraftingUI::cell_size * CraftingUI::ydim;

// texture area
const float CraftingUI::texture_width = CraftingUI::cell_size * 6;
const float CraftingUI::texture_height = CraftingUI::cell_size;

const float CraftingUI::slot_size = 32;
const float CraftingUI::cell_offset_x = 3;
const float CraftingUI::cell_offset_y = 3;
const float CraftingUI::cell_offset_x_right = 2;
const float CraftingUI::cell_offset_y_bottom = 2;
const float CraftingUI::input_offset_x = 0;
const float CraftingUI::input_offset_y = 0;
const float CraftingUI::output_offset_x = CraftingUI::cell_size * (CraftingUI::input_xdim + CraftingUI::input_output_gap);
const float CraftingUI::output_offset_y = 0;

int CraftingUI::get_grid_at(int px, int py)
{
    //pixels from upper left
    px -= xoff;
    py -= _yresf - yoff;

    if (px < 0 || px >= render_width)  return NULL_SLOT;
    if (py < 0 || py >= render_height) return NULL_SLOT;

    int xslot = px / cell_size;
    int yslot = py / cell_size;
    int slot = xslot + yslot * xdim;

    GS_ASSERT(slot < xdim*ydim);

    return slot;
}

int CraftingUI::get_slot_at(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return NULL_SLOT;
    int xslot = slot % xdim;
    int yslot = slot / xdim;

    // asserts
    int xslots_max = input_xdim + input_output_gap + output_ydim;
    GS_ASSERT(xslot < xslots_max);
    int yslots_max = (input_ydim > output_ydim) ? input_ydim : output_ydim;
    GS_ASSERT(yslot < yslots_max);

    // detect blank regions

    // in gap between input & output
    if (xslot >= input_xdim && xslot < input_xdim + input_output_gap) return NULL_SLOT;

    // in unused input region
    if (xslot < input_xdim && yslot >= input_ydim) return NULL_SLOT;

    // in unused output region
    if (xslot >= input_xdim + input_output_gap
     && xslot < input_xdim + input_output_gap + output_xdim
     && yslot >= output_ydim)
        return NULL_SLOT;

    if (this->in_craft_input_region(px,py)) slot = yslot * input_xdim + xslot;
    else
    if (this->in_craft_output_region(px,py))
    {
        xslot -= input_xdim + input_output_gap;
        slot = yslot * output_xdim + xslot;
    }
    else
    {
        GS_ASSERT(false);  // it shouldnt be anywhere else
    }
    return slot;
}

bool CraftingUI::in_craft_input_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    int xslot = slot % xdim;
    int yslot = slot / xdim;
    return (xslot < input_xdim && yslot < input_ydim);
}

bool CraftingUI::in_craft_output_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    int xslot = slot % xdim;
    int yslot = slot / xdim;
    return (xslot >= input_xdim + input_output_gap
          && xslot < input_xdim + input_output_gap + output_xdim
          && yslot < output_ydim);
}

void CraftingUI::draw()
{
    this->draw_name();

    IF_ASSERT(CraftingTexture == 0) return;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, CraftingTexture);

    glColor4ub(255, 255, 255, 255);

    float w = texture_width;
    float h = texture_height;

    float x = xoff;
    float y = yoff;

    float tx_min = 0.0f;
    float ty_min = 0.0f;
    float tx_max = texture_width/512.0f;
    float ty_max = texture_height/512.0f;

    //draw background
    glBegin(GL_QUADS);

    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x, y);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x,y-h);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x+w, y-h);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x+w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_grid_at(mouse_x, mouse_y);

    if (hover_slot != NULL_SLOT && (this->in_craft_input_region(mouse_x,mouse_y) || this->in_craft_output_region(mouse_x,mouse_y)))
    {
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
    struct ItemContainer::SlotMetadata* slot_metadata =
        ItemContainer::get_container_ui_slot_metadata(container_id);
    if (slot_metadata == NULL) return;

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    //draw input items
    for (int xslot=0; xslot<input_xdim; xslot++)
    for (int yslot=0; yslot<input_ydim; yslot++)
    {
        int slot = input_xdim*yslot + xslot;
        ItemType item_type = slot_metadata[slot].type;
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + input_offset_x + cell_offset_x + cell_size*xslot;
        const float y = yoff - (input_offset_y + cell_offset_y + cell_size*yslot);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0f/iw)*(tex_id % iiw);
        const float ty_min = (1.0f/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0f/iw;
        const float ty_max = ty_min + 1.0f/iw;

        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
    }
    glEnd();

    //draw available output items
    bool any_unavailable = false;
    for (int xslot=0; xslot<output_xdim; xslot++)
    for (int yslot=0; yslot<output_ydim; yslot++)
    {
        int slot = output_xdim*yslot + xslot;
        bool available;
        ItemType item_type = Item::get_selected_craft_recipe_type(this->container_id, slot, &available);
        if (item_type == NULL_ITEM_TYPE) continue;
        if (!available)
        {
            any_unavailable = true;
            continue;
        }

        int tex_id = Item::get_sprite_index_for_type(item_type);

        x = xoff + output_offset_x + cell_offset_x + cell_size*xslot;
        y = yoff - (output_offset_y + cell_offset_y + cell_size*yslot);

        w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        tx_min = (1.0f/iw)*(tex_id % iiw);
        ty_min = (1.0f/iw)*(tex_id / iiw);
        tx_max = tx_min + 1.0f/iw;
        ty_max = ty_min + 1.0f/iw;

        glBegin(GL_QUADS);
        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
        glEnd();
    }

    //draw unavailable output items
    // switch to greyscale texture if unavailable
    if (any_unavailable)
    {
        glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->greyscale_texture);
        for (int xslot=0; xslot<output_xdim; xslot++)
        for (int yslot=0; yslot<output_ydim; yslot++)
        {
            int slot = output_xdim*yslot + xslot;
            bool available;
            ItemType item_type = Item::get_selected_craft_recipe_type(this->container_id, slot, &available);
            if (available) continue;
            if (item_type == NULL_ITEM_TYPE) continue;

            int tex_id = Item::get_sprite_index_for_type(item_type);

            // switch to greyscale texture if unavailable
            if (!available) glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->greyscale_texture);

            x = xoff + output_offset_x + cell_offset_x + cell_size*xslot;
            y = yoff - (output_offset_y + cell_offset_y + cell_size*yslot);

            w = slot_size;
            const float iw = 16.0f; // icon_width
            const int iiw = 16; // integer icon width

            tx_min = (1.0f/iw)*(tex_id % iiw);
            ty_min = (1.0f/iw)*(tex_id / iiw);
            tx_max = tx_min + 1.0f/iw;
            ty_max = ty_min + 1.0f/iw;

            glBegin(GL_QUADS);
            glTexCoord2f(tx_min, ty_min);
            glVertex2f(x, y);

            glTexCoord2f(tx_min, ty_max);
            glVertex2f(x,y-w);

            glTexCoord2f(tx_max, ty_max);
            glVertex2f(x+w, y-w);

            glTexCoord2f(tx_max, ty_min);
            glVertex2f(x+w, y);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);

    // draw stacks
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    // inputs
    for (int xslot=0; xslot<input_xdim; xslot++)
    for (int yslot=0; yslot<input_ydim; yslot++)
    {
        const int slot = input_xdim*yslot + xslot;
        int stack = slot_metadata[slot].stack_size;
        int charges = slot_metadata[slot].charges;
        HudText::Text* text = &this->item_labels[slot];
        const float x = xoff + input_offset_x + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        const float y = yoff - (input_offset_y + cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());
        draw_slot_numbers(text, x, y, stack, charges);
    }

    // outputs
    for (int xslot=0; xslot<output_xdim; xslot++)
    for (int yslot=0; yslot<output_ydim; yslot++)
    {
        // the synthesizer store slots in dat are indexed from 0
        // it is not aware of the implementation detail we have for food
        const int slot = output_xdim*yslot + xslot;
        int stack = Item::get_selected_craft_recipe_stack(this->container_id, slot);
        HudText::Text* text = &this->item_labels[slot + this->input_slots];
        const float x = xoff + output_offset_x + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        const float y = yoff - (output_offset_y + cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());
        draw_slot_numbers(text, x, y, stack, NULL_CHARGES);
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    //draw_tracking_pixel(xoff,yoff);
    glColor4ub(255, 255, 255, 255);

}

}   // HudContainer
