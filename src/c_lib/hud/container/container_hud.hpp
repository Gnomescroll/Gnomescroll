#pragma once

#include <hud/hud.hpp>
#include <hud/container/_interface.hpp>
#include <hud/container/texture.hpp>
#include <SDL/SDL_functions.hpp>

namespace HudContainer
{

class AgentInventoryUI : public UIElement
{
    public:

    int xdim;    // slot dimensions
    int ydim;

    //static const float border = 16;       // border around entire panel
    static const float border;       // border around entire panel
    static const float inc1; // spacing between slot icons
    static const float inc2;  // border around a slot icon

    static const float slot_size;    // pixel dimension

    HudText::Text* stack_numbers;

    void draw();

    int width()
    {
        return xdim*slot_size + (xdim-1)*inc1;
    }

    int height()
    {
        return ydim*slot_size + ydim*inc1 + inc2*2;
    }

    int get_slot_at(int px, int py);

    bool point_inside(int px, int py) { return (this->get_slot_at(px,py) != NULL_SLOT); }

    void init()
    {
        GS_ASSERT(this->stack_numbers == NULL);
        
        this->xdim = ItemContainer::get_container_xdim(AGENT_INVENTORY);
        this->ydim = ItemContainer::get_container_ydim(AGENT_INVENTORY);
        
        // create HudText objects needed for stack rendering
        int max = this->xdim * this->ydim;
        this->stack_numbers = new HudText::Text[max];

        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->stack_numbers[i];
            t->set_format("%d");
            t->set_format_extra_length(11 + 1 - 2);
            t->set_color(Color(255,255,255,255));
            t->set_depth(-0.1f);
        }

        this->name.set_text("Inventory");
    }

    AgentInventoryUI() : xdim(0), ydim(0), stack_numbers(NULL)
    {}
    
    ~AgentInventoryUI()
    {
        if (this->stack_numbers != NULL) delete[] this->stack_numbers;
    }
};

const float AgentInventoryUI::border = 0;       // border around entire panel
const float AgentInventoryUI::inc1 = 5; // spacing between slot icons
const float AgentInventoryUI::inc2 = 2;  // border around a slot icon
const float AgentInventoryUI::slot_size = 32;    // pixel dimension

int AgentInventoryUI::get_slot_at(int px, int py)
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

void AgentInventoryUI::draw()
{
    //this->draw_name();
    // TODO -- stop drawing this shit upside down, so we can use the common method defined on UIElement
    HudFont::start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    this->name.set_position(this->xoff, _yresf - this->yoff + this->name.get_height() + 1);
    this->name.draw();
    HudFont::end_font_draw();

    const float w = slot_size;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_COLOR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    int g1 = 127; //color 1
    //int g2 = 80-8;  //color 2

    glBegin(GL_QUADS);
    glColor4ub(g1,g1,g1, 255); //128+64);
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size));

        glVertex2f(x-inc2,y+w+inc2);
        glVertex2f(x+w+inc2, y+w+inc2);
        glVertex2f(x+w+inc2, y-inc2);
        glVertex2f(x-inc2, y-inc2);
    }
    glEnd();

    if (this->container_id == NULL_CONTAINER) return;

    int* slot_types = ItemContainer::get_container_ui_types(this->container_id);
    int* slot_stacks = ItemContainer::get_container_ui_stacks(this->container_id);
    int* slot_durabilities = ItemContainer::get_container_ui_durabilities(this->container_id);
    if (slot_types == NULL) return;
    GS_ASSERT(slot_stacks != NULL);
    GS_ASSERT(slot_durabilities != NULL);

    // render slot backgrounds
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        const float alpha = 128;
        
        // always draw grey background
        int slot = j*xdim + i;
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size));
        float ratio = 1.0f;
        glColor4ub(80, 80, 80, alpha);    // grey
        Hud::meter_graphic.draw(x, y, w, w, ratio);

        // maybe draw a dura meter on it
        int durability = slot_durabilities[slot];
        if (durability != NULL_DURABILITY)
        {
            int max_durability = Item::get_max_durability(slot_types[slot]);
            ratio = ((float)durability)/((float)max_durability);

			int mh = w / 8; // meter height
			glColor4ub(255, 0, 0, alpha);               // red
            Hud::meter_graphic.draw(x, y, w, mh, 1.0f); // full slot width background
            Hud::set_color_from_ratio(ratio, 255);
            Hud::meter_graphic.draw(x, y, w, mh, ratio);
        }
    }
    
    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_slot_at(mouse_x, mouse_y);
    if (hover_slot != NULL_SLOT)
    {
        int i = hover_slot % this->xdim;
        int j = hover_slot / this->xdim;
        
        float x = xoff + border + i*(inc1+slot_size);
        float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }
    glEnd();

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    // draw icons
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (slot_types[slot] == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(slot_types[slot]);
        const float x = xoff + border + i*(inc1+slot_size);
        const float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size));

        //const float iw = 8.0f; // icon_width
        //const int iiw = 8; // integer icon width
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width
        
        const float tx_min = (1.0f/iw)*(tex_id % iiw);
        const float ty_min = (1.0f/iw)*(tex_id / iiw);
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

    /*
     * Draw stack numbers
     */

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    HudText::Text* text;
    for (int i=0; i<this->xdim; i++)
    for (int j=0; j<this->ydim; j++)
    {
        const int slot = j * this->xdim + i;
        int count = slot_stacks[slot];
        if (count <= 1) continue;
        
        text = &this->stack_numbers[slot];
        text->update_formatted_string(1, count);

        const float x = xoff + border + i*(inc1+slot_size) + slot_size - text->get_width();
        const float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size) - text->get_height());

        text->set_position(x,y);
        text->draw();
    }
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}

}
