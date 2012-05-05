#pragma once

namespace t_hud
{

//259x222
class AgentNaniteUI : public UIElement
{
    public:

    static const float border = 16;       // border around entire panel
    static const float inc1 = 8; // spacing between slot icons
    static const float inc2 = 2;  // border around a slot icon

    static const float slot_size = 37;    // pixel dimension


    //slots are 37 px in size
    static const int xdim = 7;    // slot dimensions
    static const int ydim = 6;


    void init() {}
    void draw();

    int get_slot_at(int px, int py);

};


int AgentNaniteUI::get_slot_at(int px, int py)
{
    px -= xoff + border;
    py -= yoff - border;

    float width  = xdim*slot_size; //fix
    float height = ydim*slot_size; //fix

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / slot_size;
    int yslot = py / slot_size;

    int slot = yslot * this->xdim + xslot;
    
    return slot;
}

void AgentNaniteUI::draw()
{


    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture( GL_TEXTURE_2D, NaniteTexture );

    glColor4ub(255, 255, 255, 255);

    const float w = xdim*slot_size; //259
    const float h = ydim*slot_size; //222

    const float x = xoff;
    const float y = yoff - h;

    const float tx_min = 0.0;
    const float ty_min = 0.0;
    const float tx_max = 259.0/512.0;
    const float ty_max = 222.0/512.0;

    glBegin(GL_QUADS);

    glTexCoord2f( tx_min, ty_min );
    glVertex2f(x,y+h);

    glTexCoord2f( tx_max, ty_min );
    glVertex2f(x+w, y+h );
        
    glTexCoord2f( tx_max, ty_max );
    glVertex2f(x+w, y);

    glTexCoord2f( tx_min, ty_max );
    glVertex2f(x, y);

    glEnd();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    glColor4ub(255, 255, 255, 255);
}

}
