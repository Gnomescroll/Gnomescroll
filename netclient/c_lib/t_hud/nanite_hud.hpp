#pragma once

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

    printf("nanite click: %i %i \n", px, py);

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

    if(xslot <= 3 && yslot < 3)
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

    glBegin(GL_QUADS);
/*
    glTexCoord2f( tx_min, ty_min );
    glVertex2f(x,y+h);

    glTexCoord2f( tx_max, ty_min );
    glVertex2f(x+w, y+h );
        
    glTexCoord2f( tx_max, ty_max );
    glVertex2f(x+w, y);

    glTexCoord2f( tx_min, ty_max );
    glVertex2f(x, y);
*/

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


    glColor4ub(255, 0, 0, 255);

    float p = 1.0;
    glBegin(GL_QUADS);

    glTexCoord2f( tx_min, ty_min );
    glVertex2f(x-p,y+p);

    glTexCoord2f( tx_max, ty_min );
    glVertex2f(x+p, y+p );
        
    glTexCoord2f( tx_max, ty_max );
    glVertex2f(x+p, y-p);

    glTexCoord2f( tx_min, ty_max );
    glVertex2f(x-p, y-p);

    glEnd();


    glColor4ub(255, 255, 255, 255);
}

}
