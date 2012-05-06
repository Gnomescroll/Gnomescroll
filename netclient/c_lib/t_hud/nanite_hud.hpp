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

    static const float inc1 = 6; // spacing between slot icons
    static const float inc2 = 2;  // border around a slot icon

    static const float slot_size = 32;    // pixel dimension

    //slots are 37 px in size

    static const int xdim = 2;    // slot dimensions
    static const int ydim = 4;

    static const float nanite_width = 222.0f;
    static const float nanite_height = 148.0f;
    static const int level = 0;    //nanite level

    static const int slot_offset_x = 149;
    static const int slot_offset_y = 1;
    static const int slot_border = 2;
    static const int slot_border_gap = 1;

    void init() {}
    void draw();

    int width()
    {
        return nanite_width;
    }

    int height()
    {
        return nanite_height;
    }

    int get_slot_at(int px, int py);
    void handle_ui_event(int px, int py);
};

int AgentNaniteUI::get_slot_at(int px, int py)
{
    //pixels from upper left
    px -= xoff + nanite_width;
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
    px -= xoff + nanite_width;
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

    if (xslot <= 3)
    {
        if (xslot == 3 && yslot == 3)
        {
            //pickup item being eaten (if it exists)
            //inventory slot 0
        }
        else
        {
            //collect nannite if ready
        }
    }



    if (xslot == 5 && yslot == 3)
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

    const float w = nanite_width;
    const float h = nanite_height;

    const float x = xoff;
    const float y = yoff;

    const float tx_min = 0.0;
    const float ty_min = 0.0;
    const float tx_max = nanite_width/512.0;
    const float ty_max = nanite_height/512.0;

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


    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    glBegin(GL_QUADS);

    //draw store items
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        if (xslot == xdim-1 && yslot == ydim-1) continue;    // this is the last slot, put money here

        int item_id, cost;
        Item::get_nanite_store_item(level, xslot, yslot, &item_id, &cost);
        //if (item_id == NULL_ITEM) continue;

        //int tex_id = Item::get_sprite_index_for_type(item_id);
        int tex_id = ERROR_SPRITE;

        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot;
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);

        const float w = 32.0f;
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

    //draw text for item cost in upper right
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        if (xslot == xdim-1 && yslot == ydim-1) continue;

        int item_id, cost;
        Item::get_nanite_store_item(level,xslot,yslot, &item_id, &cost);
        if (item_id == NULL_ITEM) continue;

        //const float x = xoff+ 37*xslot;
        //const float y = yoff- 37*yslot;

        //draw text for cost

        //cost
    }




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
