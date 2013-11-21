/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

const struct MapElement NULL_MAP_ELEMENT = {{{ EMPTY_CUBE, 0, 0, 0 }}};
const struct MapElement ABOVE_MAP_ELEMENT = {{{ EMPTY_CUBE, 0, 0x0F, 0 }}};  // max sunlight

}   // t_map
