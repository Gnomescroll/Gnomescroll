#pragma once

#include <hud/container/ui_element.hpp>
#include <SDL/draw_functions.hpp>

namespace HudContainer
{

class EquipmentUI: public UIElement
{
    public:

        static const int height = 147;
        static const int width = 256;

        static const int texture_width = 256;
        static const int texture_height = 256;

        int xdim;
        int ydim;

    virtual int get_slot_at(int px, int py)
    {
        return NULL_SLOT;
    }

    virtual bool point_inside(int px, int py)
    {
        return (px >= this->xoff && px < this->xoff + width &&
                py >= this->yoff && py < this->yoff + height);
    }

    virtual void init()
    {
        this->xdim = ItemContainer::get_container_xdim(ItemContainer::name::equipment);
        this->ydim = ItemContainer::get_container_ydim(ItemContainer::name::equipment);
    }

    virtual void draw();

    EquipmentUI() :
        xdim(0), ydim(0)
    {
    }

    virtual ~EquipmentUI()
    {

    }
};

void EquipmentUI::draw()
{
    this->draw_name();

    IF_ASSERT(EquipmentTexture == 0) return;

    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, EquipmentTexture);

    glColor4ub(255, 255, 255, 255);

    draw_bound_texture_sprite(
        this->xoff, this->yoff - this->height - 4,  // -4 to align with inventory correctly
        this->width, this->height,
        -1.0f,
        0.0f, 0.0f,
        float(this->width)/float(this->texture_width),
        float(this->height)/float(this->texture_height));

    glDisable(GL_TEXTURE_2D);
}

}   // HudContainer
