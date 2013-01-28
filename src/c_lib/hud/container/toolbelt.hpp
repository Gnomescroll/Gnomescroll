#pragma once

#include <hud/hud.hpp>
#include <hud/container/texture.hpp>

namespace HudContainer
{

class AgentToolbeltUI : public UIElement
{
    public:

        //static const float border = 16;       // border around entire panel
        static const int border = 0;       // border around entire panel
        static const int inc1 = 8; // spacing between slot icons
        static const int inc2 = 2;  // border around a slot icon

        static const int slot_size = 32;    // pixel dimension

        static const int xdim = 9;    // slot dimensions
        static const int ydim = 1;

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

    bool point_inside(int px, int py)
    {
        return (this->get_slot_at(px,py) != NULL_SLOT);
    }

    void init()
    {
        this->init_item_labels(this->xdim * this->ydim);
    }

    AgentToolbeltUI() :
        selected_slot(0)
    {}

    virtual ~AgentToolbeltUI()
    {
    }
};


int AgentToolbeltUI::get_slot_at(int px, int py)
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

void AgentToolbeltUI::draw_name()
{
    // TODO -- stop drawing this shit upside down, so we can use the common method defined on UIElement
    HudFont::start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    this->name.set_position(this->xoff + this->width() - this->name.get_width() - this->inc1/2, _yresf - this->yoff + this->name.get_height() - 3);
    this->name.draw();
    HudFont::end_font_draw();
}

void AgentToolbeltUI::draw()
{
    const float w = slot_size;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw slot border
    int intensity = 0; // old intensity == 80-16
    glBegin(GL_QUADS);
    glColor4ub(intensity, intensity, intensity, 55); // old alpha == 128+64
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
    struct ItemContainer::SlotMetadata* slot_metadata =
        ItemContainer::get_container_ui_slot_metadata(container_id);
    if (slot_metadata == NULL) return;

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
        int durability = slot_metadata[slot].durability;
        if (durability != NULL_DURABILITY)
        {
            int max_durability = Item::get_max_durability(slot_metadata[slot].type);
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
        float y = _yresf - (yoff - border + (j+1)*(inc1+slot_size));

        glVertex2f(x,y+w);
        glVertex2f(x+w, y+w);
        glVertex2f(x+w, y);
        glVertex2f(x, y);
    }

    glEnd();

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    GS_ASSERT(TextureSheetLoader::item_texture_sheet_loader->texture != 0);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        int slot = j * xdim + i;
        if (slot_metadata[slot].type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(slot_metadata[slot].type);
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

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);

    // draw border highlight
    if (this->selected_slot != NULL_SLOT)
    {
        int slotx = this->selected_slot % xdim;
        int sloty = ydim - (this->selected_slot / xdim);
        const float x = xoff + border + slotx*(inc1+slot_size);
        const float y = _yresf - (yoff + border + (ydim-sloty+1)*(inc1+slot_size));

        const float b = inc2;

        glColor4ub(0, 0, 128+64, 255);
        glLineWidth(2.0f);

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

        glLineWidth(1.0f);
    }

    glColor4ub(255, 255, 255, 255);

    /*
     * Draw stack numbers
     */

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    for (int i=0; i<this->xdim; i++)
    for (int j=0; j<this->ydim; j++)
    {
        const int slot = j * this->xdim + i;
        int stack = slot_metadata[slot].stack_size;
        int charges = slot_metadata[slot].charges;
        HudText::Text* text = &this->item_labels[slot];
        const float x = xoff + border + i*(inc1+slot_size) + slot_size - text->get_width();
        const float y = _yresf - (yoff + border + (j+1)*(inc1+slot_size) - text->get_height());
        draw_slot_numbers(text, x, y, stack, charges);
    }
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}

}   // HudContainer
