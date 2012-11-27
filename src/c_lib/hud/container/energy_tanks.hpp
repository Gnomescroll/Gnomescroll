#pragma once

#include <item/common/constants.hpp>
#include <hud/container/ui_element.hpp>

namespace HudContainer
{

class EnergyTanksUI : public UIElement
{
    public:

    static const int border = 0;       // border around entire panel
    static const int inc1 = 8; // spacing between slot icons
    static const int inc2 = 2;  // border around a slot icon

    static const int slot_size = 32;    // pixel dimension

    int xdim;    // slot dimensions
    int ydim;

    int selected_slot;

    int width()
    {
        return xdim*slot_size + (xdim-1)*inc1 + inc2*2;
    }

    int height()
    {
        return ydim*slot_size + ydim*inc1 + inc2*2;
    }

    void draw();
    void draw_name(); // TODO: fix rendering so we dont need to override UIElement's draw_name

    int get_slot_at(int px, int py);

    bool point_inside(int px, int py) { return (this->get_slot_at(px,py) != NULL_SLOT); }

    void init()
    {
        this->xdim = ItemContainer::get_container_xdim(AGENT_ENERGY_TANKS);
        this->ydim = ItemContainer::get_container_ydim(AGENT_ENERGY_TANKS);
        this->name.set_text("Energy Tanks");
    }

    EnergyTanksUI() : xdim(0), ydim(0), selected_slot(0)
    {}
    
    ~EnergyTanksUI()
    {
    }
};


int EnergyTanksUI::get_slot_at(int px, int py)
{
    px -= xoff - border - inc1/2;
    py -= yoff + border + inc1/2;

    float width  = xdim*slot_size + (xdim-1)*inc1 + inc2*2;
    float height = ydim*slot_size + (ydim-1)*inc1 + inc2*2;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (inc1 + slot_size);
    int yslot = py / (inc1 + slot_size);

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
    this->name.set_position(this->xoff, _yresf - this->height() /*- this->name.get_height()*/ );
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

    //int g1 = 80-16; //color 1

    // draw alpha quads behind tanks
    //glBegin(GL_QUADS);
 //   glColor4ub(g1,g1,g1, 128+64); //128+64);
 //   for (int i=0; i<xdim; i++)
 //   for (int j=0; j<ydim; j++)
 //   {
 //       float x = xoff + border + i*(inc1+w);
 //       float y = _yresf - (yoff + border + (j+1)*(inc1+w));

 //       glVertex2f(x-inc2,y+w+inc2);
 //       glVertex2f(x+w+inc2, y+w+inc2);
 //       glVertex2f(x+w+inc2, y-inc2);
 //       glVertex2f(x-inc2, y-inc2);
 //   }
 //   glEnd();

    if (this->container_id == NULL_CONTAINER) return;
    int* slot_types = ItemContainer::get_container_ui_types(this->container_id);
    GS_ASSERT(slot_types != NULL);
    if (slot_types == NULL) return;
    
    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    if (hover_slot != NULL_SLOT)
    {
        int i = hover_slot % this->xdim;
        int j = hover_slot / this->xdim;
        
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff - border + (j+1)*(inc1+slot_size));

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

    GS_ASSERT(TextureSheetLoader::GreyScaleItemTexture != 0);
    if (TextureSheetLoader::GreyScaleItemTexture == 0) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::GreyScaleItemTexture);

    // draw unloaded energy tanks as greyscale
    // (here's where we ALSO used the LOADED drawing code below, but with the following line changed
    // in order to only draw the empty tanks:
    //         if (slot_types[slot] != NULL_ITEM_TYPE) continue;
    
    GS_ASSERT(TextureSheetLoader::ItemSheetTexture != 0);
    if (TextureSheetLoader::ItemSheetTexture == 0)
    {
        glDisable(GL_TEXTURE_2D);
        return;
    }
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::ItemSheetTexture);

    // count loaded energy tanks, to figure appropriate (centered) xoff
    int num_loaded = 0;
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        if (slot_types[j * xdim + i] != NULL_ITEM_TYPE) 
            num_loaded++;
    }
    xoff = (_xresf - num_loaded * slot_size) / 2;

    // draw loaded energy tanks
    glBegin(GL_QUADS);
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (slot_types[slot] == NULL_ITEM_TYPE) continue;
        GS_ASSERT(slot_types[slot] == energy_tank_type);
        const float x = xoff + /*border +*/ i * (/*inc1 + */slot_size);
        const float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size));

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
