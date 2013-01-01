#pragma once

namespace t_map
{

struct MAP_ELEMENT
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

const struct MAP_ELEMENT NULL_MAP_ELEMENT = {{{ 0,0,0,0 }}};
}   // t_map
