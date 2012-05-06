#pragma once

#include <item/properties.hpp>

namespace t_hud
{

//Item
//void get_nanite_store_item(int level, int slotx, int sloty, int* item_id, int* cost)

/*
enum NaniteUIregions
{
    ItemPickup,
    ItemDropoff,
    NannitePickup,
    NaniteDropoff,
    NaniteCollection
}
*/

//221x147

void u_dot(float x, float y);

class AgentNaniteUI : public UIElement
{
    public:

    static const float inc1 = 8; // spacing between slot icons
    static const float inc2 = 2;  // border around a slot icon

    static const float slot_size = 37;    // pixel dimension


    //slots are 37 px in size

    static const int xdim = 6;    // slot dimensions
    static const int ydim = 4;

    void init() {}
    void draw();

    int get_slot_at(int px, int py);
    void handle_ui_event(int px, int py);
};

int AgentNaniteUI::get_slot_at(int px, int py)
{
    //pixels from upper left
    px -= xoff;
    py = yoff - (_yresf - py);

    //py -= yoff;

    float width  = xdim*slot_size; //221
    float height = ydim*slot_size; //147

    //printf("nanite click: %i %i \n", px, py);

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    //printf("nanite click: %i %i \n", px, py);

    int xslot = px / slot_size;
    int yslot = py / slot_size;

    printf("nanite slot: %i %i \n", xslot, yslot);

    //int slot = yslot * this->xdim + xslot;
    
    //return slot;
    return NULL_SLOT;
}

void AgentNaniteUI::handle_ui_event(int px, int py)
{
    px -= xoff;
    py = yoff - (_yresf - py);

    float width  = xdim*slot_size; //fix
    float height = ydim*slot_size; //fix

    if (px < 0 || px > width)  return;
    if (py < 0 || py > height) return;

    int xslot = px / slot_size;
    int yslot = py / slot_size;

    //int slot = yslot * this->xdim + xslot;

    //nanite region
    assert(xslot >= 0 && xslot < xdim && yslot >= 0 && yslot < ydim);

    if(xslot <= 3)
    {
        if(xslot == 3 && yslot == 3)
        {
            //pickup item being eaten (if it exists)
            //inventory slot 0
        }
        else
        {
            //collect nannite if ready
        }
    }



    if(xslot == 5 && yslot == 3)
    {
        //pickup nannites from hand
        //dropoff nannites from hand

        //inventory slot 1
    }
    else
    {
        //item purchase
    }


}

//221x147
void AgentNaniteUI::draw()
{


    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture( GL_TEXTURE_2D, NaniteTexture );

    glColor4ub(255, 255, 255, 255);

    const float w = xdim*slot_size; //221
    const float h = ydim*slot_size; //147

    const float x = xoff;
    const float y = yoff;

    const float tx_min = 0.0;
    const float ty_min = 0.0;
    const float tx_max = 221.0/512.0;
    const float ty_max = 147.0/512.0;

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

    //draw store items

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);
    
    for (int xslot=4; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        if(xslot == xdim-1 && yslot == ydim-1) continue;

        //if (slot_types[slot] == NULL_ITEM_TYPE) continue;

        const int level = 0;
        int item_id;
        int cost;
        Item::get_nanite_store_item(level,xslot,yslot, &item_id, &cost);

        if(item_id == -1 ) continue;
        int tex_id = Item::get_sprite_index_for_type(item_id);

        const float x = xoff+ 37*xslot;
        const float y = yoff- 37*yslot;

        const float w = 32.0;

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



    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    u_dot(x,y);
    glColor4ub(255, 255, 255, 255);
}


void u_dot(float x, float y)
{
    /*
        Draw dot in upper left corner
    */
    glDisable(GL_TEXTURE_2D);
    glColor4ub(255, 0, 0, 255);

    float p = 1.0;
    glBegin(GL_QUADS);

    glVertex2f(x-p,y+p);
    glVertex2f(x+p, y+p );
    glVertex2f(x+p, y-p);
    glVertex2f(x-p, y-p);

    glEnd();
}

}
