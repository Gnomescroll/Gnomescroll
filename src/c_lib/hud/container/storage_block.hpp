#pragma once

#include <item/properties.hpp>

namespace HudContainer
{

class StorageBlockUI : public UIElement
{
    public:
          
    static const int cell_size = 37;
    int xdim;    // grid cell size
    int ydim;

    // size of texture/render area
    float render_width;
    float render_height;

    static const int slot_size = 32;
    static const int cell_offset_x = 3;
    static const int cell_offset_y = 3;
    static const int cell_offset_x_right = 2;
    static const int cell_offset_y_bottom = 2;

    float texture_offset_x;
    float texture_offset_y;

    HudText::Text* stacks;

    bool centered;

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

    void init()
    {
        this->init_text();
        this->refresh_render_size();
        if (this->centered) this->center();
        this->name.set_text("Storage Block");
    }

    void init_text()
    {
        if (this->stacks != NULL)
        {
            delete[] this->stacks;
            this->stacks = NULL;
        }
        
        int max = xdim * ydim;
        GS_ASSERT(max > 0);
        if (max <= 0) return;
        this->stacks = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->stacks[i];
            t->set_format("%d");
            t->set_format_extra_length(11 + 1 - 2);
            t->set_color(255,255,255,255);
            t->set_depth(-0.1f);
        }
    }

    void center()
    {
        int x = _xresf - this->width();
        float off = 0;
        if (x % 2) off = 1;
        this->xoff = x/2 + off;
    }

    void refresh_render_size()
    {
        this->render_width = this->cell_size * this->xdim;
        this->render_height = this->cell_size * this->ydim;
    }

    void set_container_type(ItemContainerType container_type)
    {
        if (this->container_type == container_type) return;
        
        this->container_type = container_type;

        this->xdim = ItemContainer::get_container_xdim(container_type);
        this->ydim = ItemContainer::get_container_ydim(container_type);

        switch (container_type)
        {
            case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
                this->texture = &StorageBlockTexture;
                this->texture_offset_x = 0.0f;
                this->texture_offset_y = 0.0f;
                break;

            case CONTAINER_TYPE_CRYOFREEZER_SMALL:
                this->texture = &StorageBlockTexture;
                this->texture_offset_x = 0.0f;
                this->texture_offset_y = 0.0f;
                break;

            default:
                GS_ASSERT(false);
                break;
        }
        this->init_text();
        this->refresh_render_size();
        if (this->centered) this->center();
    }

    StorageBlockUI()
    : xdim(1), ydim(1),
    render_width(1.0f), render_height(1.0f),
    texture_offset_x(0.0f), texture_offset_y(0.0f),
    stacks(NULL),
    centered(true)
    {}

    ~StorageBlockUI()
    {
        if (this->stacks != NULL) delete[] this->stacks;
    }
};

int StorageBlockUI::get_slot_at(int px, int py)
{  
    //pixels from upper left
    px -= xoff;
    py -= _yresf - yoff;

    if (px < 0 || px >= render_width)  return NULL_SLOT;
    if (py < 0 || py >= render_height) return NULL_SLOT;

    int xslot = px / cell_size;
    int yslot = py / cell_size;
    int slot = xslot + yslot * xdim;

    return slot;
}

void StorageBlockUI::draw()
{
    GS_ASSERT(this->texture != NULL);
    if (this->texture == NULL) return;
    GS_ASSERT(*this->texture != 0);
    if (*this->texture == 0) return;
 
    this->draw_name();
    
    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, *this->texture);

    glColor4ub(255, 255, 255, 255);

    float w = render_width;
    float h = render_height;

    float x = xoff;
    float y = yoff;

    float tx_min = texture_offset_x;
    float ty_min = texture_offset_y;
    float tx_max = render_width/256.0f;
    float ty_max = render_height/256.0f;

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
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    
    if (hover_slot != NULL_SLOT)
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
    int* slot_types = ItemContainer::get_container_ui_types(this->container_id);
    int* slot_stacks = ItemContainer::get_container_ui_stacks(this->container_id);
    if (slot_types == NULL) return;
    GS_ASSERT(slot_stacks != NULL);
    if (slot_stacks == NULL) return;

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    //draw items
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        int slot = xdim*yslot + xslot;
        int item_type = slot_types[slot];
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + cell_offset_x + cell_size*xslot;
        const float y = yoff - (cell_offset_y + cell_size*yslot);

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

    glDisable(GL_TEXTURE_2D);

    // draw stacks
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    HudText::Text* text;
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        // the synthesizer store slots in dat are indexed from 0
        // it is not aware of the implementation detail we have for food
        const int slot = xdim*yslot + xslot;

        int stack = slot_stacks[slot];
        if (stack <= 1) continue;

        GS_ASSERT(count_digits(stack) < STACK_COUNT_MAX_LENGTH);

        text = &this->stacks[slot];
        text->update_formatted_string(1, stack);

        const float x = xoff + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        const float y = yoff - (cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());

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

}   // HudContainer
