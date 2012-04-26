#pragma once

#include <c_lib/t_hud/constants.hpp>

namespace t_hud
{


class UIElement
{
        public:

        bool visible;
        float xoff;
        float yoff;

        int inventory_id;
        int selected_slot; //how is this set?

        void set_position(float x, float y)
        {
            xoff = x;
            yoff = y;
        }

        virtual void init() = 0;
        virtual void draw() = 0;
        virtual int get_slot_at(int px, int py) = 0;

        UIElement()
        : visible(false), xoff(0), yoff(0), inventory_id(-1), selected_slot(NULL_SLOT)
        {}
    
        virtual ~UIElement() {} // required of all ABCs
};

} // t_hud
