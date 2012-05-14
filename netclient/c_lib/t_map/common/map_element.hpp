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
            unsigned char damage;
            unsigned char lighting;
        };
        unsigned int n;
    };
};

}