#pragma once

#include <item/common/constants.hpp>
#include <hud/container/ui_element.hpp>

namespace HudContainer
{

class EnergyTanksUI : public UIElement
{
    public:

        static const int border = 0;       // border around entire panel
        static const int span_tween_slots = 8;
        static const int inc2 = 2;  // border around a slot icon

        static const int slot_size = 32;    // pixel dimension

        bool inv_open;

        int xdim;    // slot dimensions
        int ydim;

        int selected_slot;

    int count()
    { // count loaded energy tanks
        if (this->container_id == NULL_CONTAINER) return 0;
        struct ItemContainer::SlotMetadata* slot_metadata =
            ItemContainer::get_container_ui_slot_metadata(container_id);
        IF_ASSERT(slot_metadata == NULL) return 0;

        int num_loaded = 0;

        for (int i=0; i<xdim; i++)
        for (int j=0; j<ydim; j++)
        {
            if (slot_metadata[j * xdim + i].type != NULL_ITEM_TYPE) 
                num_loaded++;
        }

        return num_loaded;
    }

    int width()
    {
        return xdim*slot_size + (xdim-1)*span_tween_slots + inc2*2;
    }

    int height()
    {
        return ydim*slot_size + ydim*span_tween_slots + inc2*2;
    }

    void draw();
    void draw_name(); // TODO: fix rendering so we dont need to override UIElement's draw_name

    int get_slot_at(int px, int py);

    bool point_inside(int px, int py) { return (this->get_slot_at(px,py) != NULL_SLOT); }

    void init()
    {
        this->inv_open = false;
        this->xdim = ItemContainer::get_container_xdim(ItemContainer::name::energy_tanks);
        this->ydim = ItemContainer::get_container_ydim(ItemContainer::name::energy_tanks);
    }

    EnergyTanksUI() :
        xdim(0), ydim(0), selected_slot(0)
    {}
    
    virtual ~EnergyTanksUI()
    {
    }
};


int EnergyTanksUI::get_slot_at(int px, int py)
{
    px -= xoff - border - span_tween_slots/2;
    py -= yoff + border + span_tween_slots/2;

    float width  = xdim*slot_size + (xdim-1)*span_tween_slots + inc2*2;
    float height = ydim*slot_size + (ydim-1)*span_tween_slots + inc2*2;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (span_tween_slots + slot_size);
    int yslot = py / (span_tween_slots + slot_size);

    int slot = yslot * this->xdim + xslot;
    
    return slot;
}

void EnergyTanksUI::draw_name()
{
    // TODO -- stop drawing this shit upside down, so we can use the common method defined on UIElement
    HudFont::start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    // this->name.set_position(this->xoff, _yresf - this->yoff + this->name.get_height() - 3);
    this->name.set_position(this->xoff, _yresf - this->height() );
    this->name.draw();
    HudFont::end_font_draw();
}

void EnergyTanksUI::draw()
{
    const float w = slot_size;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (this->container_id == NULL_CONTAINER) return;
    
    struct ItemContainer::SlotMetadata* slot_metadata =
        ItemContainer::get_container_ui_slot_metadata(container_id);
    IF_ASSERT(slot_metadata == NULL) return;

    if (inv_open) 
    {
        // draw larger rect for border of slot
        int g1 = 80-16;
        glColor4ub(1,1,1, 128+64); //128+64);
        glBegin(GL_QUADS);
        for (int i=0; i<xdim; i++)
        for (int j=0; j<ydim; j++)
        {
            float x = xoff + border + i*(span_tween_slots+w);
            float y = _yresf - (yoff + border + (j+1)*(span_tween_slots+w));

            glVertex2f(x-inc2,y+w+inc2);
            glVertex2f(x+w+inc2, y+w+inc2);
            glVertex2f(x+w+inc2, y-inc2);
            glVertex2f(x-inc2, y-inc2);
        }
        glEnd();

        // draw slot rect behind tanks
        glColor4ub(g1,g1,g1, 128+64); //128+64);
        glBegin(GL_QUADS);
        for (int i=0; i<xdim; i++)
        for (int j=0; j<ydim; j++)
        {
            float x = xoff + border + i*(span_tween_slots+w);
            float y = _yresf - (yoff + border + (j+1)*(span_tween_slots+w));

            glVertex2f(x,y+w);
            glVertex2f(x+w, y+w);
            glVertex2f(x+w, y);
            glVertex2f(x, y);
        }
        glEnd();
    }

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    if (hover_slot != NULL_SLOT)
    {
        int i = hover_slot % this->xdim;
        int j = hover_slot / this->xdim;
        
        float x = xoff + border + i*(span_tween_slots+w);
        float y = _yresf - (yoff - border + (j+1)*(span_tween_slots+w));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }

    glEnd();
    glColor4ub(255, 255, 255, 255);
    
    int energy_tank_type = Item::get_item_type("energy_tank");
    GS_ASSERT(energy_tank_type != NULL_ITEM_TYPE);
    int energy_tank_sprite_index = Item::get_sprite_index_for_type(energy_tank_type);
    GS_ASSERT(energy_tank_sprite_index != ERROR_SPRITE);

    IF_ASSERT(TextureSheetLoader::item_texture_sheet_loader->greyscale_texture == 0) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->greyscale_texture);

    // draw unloaded energy tanks as greyscale
    // (here's where we ALSO used the LOADED tank drawing code below, but with the following line changed
    // in order to only draw the empty tanks:
    //         if (slot_metadata[slot].type != NULL_ITEM_TYPE) continue;
    
    IF_ASSERT(TextureSheetLoader::item_texture_sheet_loader->texture == 0)
    {
        glDisable(GL_TEXTURE_2D);
        return;
    }
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    xoff = (_xresf - count() * slot_size) / 2;

    // draw loaded energy tanks
    glBegin(GL_QUADS);
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (slot_metadata[slot].type == NULL_ITEM_TYPE) continue;
        GS_ASSERT(slot_metadata[slot].type == energy_tank_type);
        const float x = xoff + border + i * (span_tween_slots + w);
        const float y = _yresf - (yoff + border + (j+1)*(span_tween_slots + w));

        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width
        
        const float tx_min = (1.0f/iw)*(energy_tank_sprite_index % iiw);
        const float ty_min = (1.0f/iw)*(energy_tank_sprite_index / iiw);
        const float tx_max = tx_min + 1.0f/iw;
        const float ty_max = ty_min + 1.0f/iw;

        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x,y+w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y+w);
            
        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x, y);
    }

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

}   // HudContainer
