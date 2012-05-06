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

const int ITEM_PRICE_MAX_LENGTH = 4;

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

    static const int level = 0;    //nanite level

    // size of texture/render area
    static const float render_width = 222.0f;
    static const float render_height = 148.0f;

    // nanite region data
    static const int nanite_offset_x = 1;
    static const int nanite_offset_y = 1;
    static const int nanite_border = 2;
    static const int nanite_width = 143;
    static const int nanite_height = 143;
    
    // slot region data
    static const int slot_offset_x = 149;   // offset to beginning of slot area border
    static const int slot_offset_y = 1;
    static const int slot_border = 2;
    static const int slot_border_gap = 1;

    HudText::Text* prices;

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
    void handle_ui_event(int px, int py);

    void init()
    {
        assert(this->prices == NULL);
        
        int max = (xdim * ydim) - 1;    // last slot is reserved
        this->prices = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->prices[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(ITEM_PRICE_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }
    }

    AgentNaniteUI() : prices(NULL)
    {}

    ~AgentNaniteUI()
    {
        if (this->prices != NULL) delete[] this->prices;
    }
};

int AgentNaniteUI::get_slot_at(int px, int py)
{
    // check nanite region
    int nx = px - (xoff + nanite_offset_x + nanite_border);
    int ny = py - (_yresf - yoff + nanite_offset_y + nanite_border);

    if (nx >= 0 && nx < nanite_width && ny >= 0 && ny < nanite_height)
    {
        //printf("In nanite region\n");
        return NULL_SLOT;
    }
    
    //pixels from upper left
    px -= xoff + slot_offset_x;
    py -= _yresf - yoff + slot_offset_y;

    float width  = xdim*slot_size + xdim*slot_border*2 + (xdim-1)*slot_border_gap;
    float height = ydim*slot_size + ydim*slot_border*2 + (ydim-1)*slot_border_gap;

    if (px < 0 || px > width)  return NULL_SLOT;
    if (py < 0 || py > height) return NULL_SLOT;

    int xslot = px / (slot_size + slot_border*2 + slot_border_gap);
    int yslot = py / (slot_size + slot_border*2 + slot_border_gap);
    int slot = xslot + yslot * xdim;

    return slot;
}

void AgentNaniteUI::handle_ui_event(int px, int py)
{
    // check if in nanite region
    // check if in slot region

    px -= xoff + render_width;
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

    const float w = render_width;
    const float h = render_height;

    const float x = xoff;
    const float y = yoff;

    const float tx_min = 0.0;
    const float ty_min = 0.0;
    const float tx_max = render_width/512.0;
    const float ty_max = render_height/512.0;

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
        if (item_id == NULL_ITEM) continue;
        int tex_id = Item::get_sprite_index_for_type(item_id);

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

    glDisable(GL_TEXTURE_2D);

    //draw text for item cost in upper right
    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    HudText::Text* text;
    for (int xslot=0; xslot<xdim; xslot++)
    for (int yslot=0; yslot<ydim; yslot++)
    {
        const int slot = yslot*xdim + xslot;
        if (slot == xdim*ydim-1) continue;  // skip last slot, reserved

        int item_id, cost;
        Item::get_nanite_store_item(level, xslot, yslot, &item_id, &cost);
        if (item_id == NULL_ITEM) continue;

        assert(count_digits(cost) < ITEM_PRICE_MAX_LENGTH);

        text = &this->prices[slot];
        text->update_formatted_string(1, cost);

        const float x = xoff + slot_offset_x + slot_border*(2*xslot + 1) + slot_border_gap*xslot + slot_size*xslot + slot_size - text->get_width();
        const float y = yoff - (slot_offset_y + slot_border*(2*yslot + 1) + slot_border_gap*yslot + slot_size*yslot);
        
        text->set_position(x,y);
        text->draw();
    }
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    u_dot(xoff,yoff);
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
