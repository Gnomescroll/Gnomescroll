#pragma once

#if DC_CLIENT

#include <c_lib/objects/components/minivox/minivox.hpp>

const int COLORED_MINIVOX_LIST_MAX = 1024;

class ColoredMinivoxList: public BehaviourList
{
    private:
        const char* name() { return "ColoredMinivoxList"; }
    public:
        void draw();

    ColoredMinivoxList()
    : BehaviourList(COLORED_MINIVOX_LIST_MAX)
    {}
};

const int TEXTURED_MINIVOX_LIST_MAX = 1024;

class TexturedMinivoxList: public BehaviourList
{
    private:
        const char* name() { return "TexturedMinivoxList"; }
    public:
        void draw();

    TexturedMinivoxList()
    : BehaviourList(TEXTURED_MINIVOX_LIST_MAX)
    {}
};

#endif
