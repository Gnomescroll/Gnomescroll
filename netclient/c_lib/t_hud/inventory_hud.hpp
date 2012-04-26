#pragma once

#include <c_lib/t_hud/_interface.hpp>

#include <c_lib/t_hud/texture.hpp>

#include <c_lib/SDL/SDL_functions.h>

namespace t_hud
{


class AgentInventoryUI : public UIElement
{
    public:

    static const float border = 16;       // border around entire panel
    static const float inc1 = 8; // spacing between slot icons
    static const float inc2 = 2;  // border around a slot icon

    static const float slot_size = 32;    // pixel dimension

    static const int xdim = 6;    // slot dimensions
    static const int ydim = 3;


    void init() {}
    void draw();

    void draw_background();
    void draw_slots();
    void draw_selected_slot();

    int get_slot_at(int px, int py);
};


int AgentInventoryUI::get_slot_at(int px, int py)
{
    px -= xoff + border;
    py -= yoff - border;

    float width  = xdim*slot_size + (xdim-1)*inc1;
    float height = ydim*slot_size + (ydim-1)*inc1;

    if (px < 0 || px > width)  return -1;
    if (py < 0 || py > height) return -1;

    int xslot = px / (inc1 + slot_size);
    int yslot = py / (inc1 + slot_size);

    int slot = yslot * this->xdim + xslot;
    
    return slot;
}

void AgentInventoryUI::draw() {}

void AgentInventoryUI::draw_background()
{
    const float w = slot_size;

    //glDisable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_TEXTURE_2D);

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//#if 0
    //glColor4ub(0, 0, 50, 64);

    //{
        //float x = inventory_hud_x_off;
        //float y = inventory_hud_y_off;
        //float w = 2*border + xdim*slot_size+ (xdim-1)*inc1;
        //float h = 2*border + ydim*slot_size+ (ydim-1)*inc1;

        //glBegin(GL_QUADS);

        //glVertex2f(x, y+h, z);
        //glVertex2f(x+w, y+h ,z);
        //glVertex2f(x+w, y, z);
        //glVertex2f(x, y, z);

        //glEnd();

    //}
//#endif

    glBegin(GL_QUADS);
    glColor4ub(50, 50, 50, 128);
    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + j*(inc1+slot_size));

        glVertex2f(x-inc2,y+w+inc2);
        glVertex2f(x+w+inc2, y+w+inc2);
        glVertex2f(x+w+inc2, y-inc2);
        glVertex2f(x-inc2, y-inc2);
    }

    glColor4ub(50, 50, 50, 64);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
    
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + j*(inc1+slot_size));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }

    // draw hover highlight
    glColor4ub(160, 160, 160, 128 + 64);
    int hover_slot = NULL_SLOT;
    if (active_inventory != NULL)
        hover_slot = this->get_slot_at(mouse_x, mouse_y);

    bool different_inventory = active_inventory == NULL || active_inventory->inventory_id != this->inventory_id;
    bool different_slot = different_inventory || hover_slot != active_slot;

    if (different_slot && hover_slot != NULL_SLOT)
    {
        int i = hover_slot % this->xdim;
        int j = hover_slot / this->xdim;
        
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + j*(inc1+slot_size));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }

    glEnd();

}

void AgentInventoryUI::draw_slots()
{
    const float w = slot_size;

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    InventorySlot* contents = Items::get_inventory_contents(this->inventory_id);
    if (contents == NULL) return;

    glBegin(GL_QUADS);

    int skip_slot = NULL_SLOT;
    if (active_inventory != NULL && this->inventory_id == active_inventory->inventory_id)
        skip_slot = active_slot;

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (slot == skip_slot) continue;
        if (contents[slot].item_id == EMPTY_SLOT) continue;
        int tex_id = contents[slot].sprite_index;

        const float x = xoff + border + i*(inc1+slot_size);
        const float y = _yresf - (yoff + border + j*(inc1+slot_size));

        //const float iw = 8.0f; // icon_width
        //const int iiw = 8; // integer icon width
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width
        
        const float tx_min = (1.0/iw)*(tex_id % iiw);
        const float ty_min = (1.0/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glTexCoord2f( tx_min, ty_min );
        glVertex2f(x,y+w);

        glTexCoord2f( tx_max, ty_min );
        glVertex2f(x+w, y+w );
            
        glTexCoord2f( tx_max, ty_max );
        glVertex2f(x+w, y);

        glTexCoord2f( tx_min, ty_max );
        glVertex2f(x, y);
    }

    glEnd();

    //glEnable(GL_DEPTH_TEST); // move render somewhere
    //glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);
}

void AgentInventoryUI::draw_selected_slot()
{
    const float w = slot_size;
    
    // draw border highlight
    if(this->selected_slot != NULL_SLOT)
    {   
        int slotx = this->selected_slot % xdim;
        int sloty = ydim - (this->selected_slot / xdim);
        const float x = xoff + border + slotx*(inc1+slot_size);
        const float y = _yresf - (yoff + border + (ydim-sloty)*(inc1+slot_size));

        const float b = 2 + inc2;

        glColor4ub(0, 0, 128+64, 255);
        glLineWidth(2.0);

        glBegin(GL_LINES);

        glVertex2f(x-b, y+w+b);
        glVertex2f(x+w+b, y+w+b);

        glVertex2f(x+w+b, y+w+b);
        glVertex2f(x+w+b, y-b);

        glVertex2f(x+w+b, y-b);
        glVertex2f(x-b, y-b);

        glVertex2f(x-b, y-b);
        glVertex2f(x-b, y+w+b);

        glEnd();

        glLineWidth(1.0);
    }

    glColor4ub(255, 255, 255, 255);
}


}
