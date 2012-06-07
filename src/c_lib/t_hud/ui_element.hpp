#pragma once

#include <t_hud/constants.hpp>
#include <hud/text.hpp>

namespace t_hud
{

class UIElement
{
        public:

        UIElementType type;

        bool visible;
        float xoff;
        float yoff;

        int container_id;
        int container_type;

        GLuint* texture;

        void set_position(float x, float y)
        {
            xoff = x;
            yoff = y;
        }

        virtual void init() = 0;
        
        virtual void draw() = 0;
        virtual int get_slot_at(int px, int py) = 0;
        virtual bool point_inside(int px, int py) = 0;

        virtual void set_container_type(int container_type) { this->type = type; }

        UIElement()
        :
        type(UI_ELEMENT_NONE),
        visible(false), xoff(0), yoff(0),
        container_id(NULL_CONTAINER), container_type(CONTAINER_TYPE_NONE),
        texture(NULL)
        {}
    
        virtual ~UIElement() {} //abc virtual deconstructor
};

} // t_hud
