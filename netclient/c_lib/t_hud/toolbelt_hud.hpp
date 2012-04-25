#pragma once

#include <c_lib/t_hud/texture.hpp>

namespace t_hud
{

class AgentToolbelt : public UIElement
{
    public:

    static const float border = 16;       // border around entire panel
    static const float inc1 = 8; // spacing between slot icons
    static const float inc2 = 2;  // border around a slot icon

    static const float slot_size = 32;    // pixel dimension

    static const int xdim = 1;    // slot dimensions
    static const int ydim = 9;


    void init() {}
    void draw();

    bool get_slot_at(int px, int py, int* slotx, int* sloty);

};


bool AgentToolbelt::get_slot_at(int px, int py, int* slotx, int* sloty)
{
    return false;

/*
    px -= this->x + this->border;
    py -= this->y - this->border;

    float width  = xdim*slot_size + (xdim-1)*icon_spacing;
    float height = ydim*slot_size + (ydim-1)*icon_spacing;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (icon_spacing + slot_size);
    int yslot = py / (icon_spacing + slot_size);

    int slot = yslot * this->xdim + xslot;
    
    //printf("inventory: slot %d, %d\n", xslot, yslot);

    return slot;
*/
}

void AgentToolbelt::draw()
{
    const float w = slot_size;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
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
    
        float x = yoff + border + i*(inc1+slot_size);
        float y = _yresf - (ydim + border + j*(inc1+slot_size));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }

    glEnd();

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    InventorySlot* contents = Items::get_inventory_contents(this->inventory_id);
    if (contents == NULL) return;

    glBegin(GL_QUADS);

    for(int i=0; i<xdim; i++)
    for(int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (contents[slot].item_id == EMPTY_SLOT) continue;
        int tex_id = contents[slot].sprite_index;

        const float x = xoff + border + i*(inc1+slot_size);
        const float y = _yresf - (ydim + border + j*(inc1+slot_size));

        const float tx_min = (1.0/8.0)*(tex_id % 8);
        const float ty_min = (1.0/8.0)*(tex_id / 8);
        const float tx_max = tx_min + 1.0/8.0;
        const float ty_max = ty_min + 1.0/8.0;

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

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    // draw border highlight
    if(this->selected_slot != NULL_SLOT)
    {   
        int slotx = this->selected_slot % xdim;
        int sloty = ydim - (this->selected_slot / xdim);
        const float x = this->x + border + slotx*(inc1+slot_size);
        const float y = _yresf - (ydim + border + (ydim-sloty)*(inc1+slot_size));

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
