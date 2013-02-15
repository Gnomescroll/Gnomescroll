#pragma once

namespace t_map
{

struct MapElement
{
    union
    {
        struct
        {
            unsigned char block;
            unsigned char palette;
            unsigned char light;
            unsigned char damage;
            //unsigned char lighting;  //goes in another array
        };
        unsigned int n;
    };
};

const struct MapElement NULL_MAP_ELEMENT = {{{ 0,0,0,0 }}};

const struct MapElement ABOVE_MAP_ELEMENT = {{{ 0,0,0x0f,0 }}}; //max sunlight
}   // t_map
