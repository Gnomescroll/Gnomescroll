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
            unsigned char color;
            unsigned char damage;
            //unsigned char lighting;  //goes in another array
        };
        unsigned int n;
    };
};

}