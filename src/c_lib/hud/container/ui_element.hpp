#pragma once

#include <hud/container/constants.hpp>
#include <hud/text.hpp>
#include <hud/font.hpp>

namespace HudContainer
{

void draw_slot_numbers(HudText::Text* text, float x, float y, int stack_size, int charges)
{
    int val = stack_size;
    Color color = Color(255,255,255);
    if (charges != NULL_CHARGES)
    {
        val = charges;
        if (val)
            color = Color(20, 200, 200);
        else
            color = Color(200, 20, 20);
    }
    else if (val == 1)
        return;

    text->set_color(color);
    text->update_formatted_string(1, val);
    text->set_position(x, y);
    text->draw();
}

class UIElement
{
    public:

        UIElementType type;

        ItemContainerID container_id;
        ItemContainerType container_type;

        bool visible;
        float xoff;
        float yoff;

        GLuint* texture;

        HudText::Text name;

    void set_position(float x, float y)
    {
        this->xoff = x;
        this->yoff = y;
    }

    virtual void init() = 0;
    virtual void draw() = 0;
    virtual int get_slot_at(int px, int py) = 0;
    virtual bool point_inside(int px, int py) = 0;

    virtual void draw_name()
    {
        HudFont::start_font_draw();
        HudFont::reset_default();
        HudFont::set_texture();
        this->name.set_position(this->xoff, this->yoff + this->name.get_height() + 2);
        this->name.draw();
        HudFont::end_font_draw();
    }

    virtual void set_container_type(ItemContainerType container_type)
    {
        this->container_type = container_type;
    }

    virtual void set_container_id(ItemContainerID container_id)
    {
        this->container_id = container_id;
    }

    UIElement() :
        type(UI_ELEMENT_NONE),
        container_id(NULL_CONTAINER), container_type(NULL_CONTAINER_TYPE),
        visible(false), xoff(0), yoff(0),
        texture(NULL)
    {}

    virtual ~UIElement() {} //abc virtual deconstructor
};

} // HudContainer
