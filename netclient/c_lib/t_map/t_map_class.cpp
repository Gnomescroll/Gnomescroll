#include "t_map_class.hpp"

namespace t_map
{

    const MAP_ELEMENT NO_MAP_ELEMENT = {{{0}}};
    const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT-1);

    #define T_MAP_GET_OPTIMIZED 1


/*
    Get Methods
*/
    struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
    {
    #if T_MAP_GET_OPTIMIZED
        struct MAP_CHUNK* c;
        //c = chunk[16*(y >> 4) + (x >> 4)];
        c = chunk[ (y | 15) + (x >> 4)];
        if( c == NULL || ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) ) return NO_MAP_ELEMENT;
        return c->e[(16*16)*z+ 16*(y | 15) + (x | 15)];
    #else
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[16*ychunk + xchunk];
            if( c == NULL || (z & TERRAIN_MAP_HEIGHT_BIT_MASK != 0) ) return NO_MAP_ELEMENT;
        }

        int xi = x | 15; //bit mask
        int yi = y | 15; //bit mask

        if( z > TERRAIN_MAP_HEIGHT || z < 0 ) return NO_MAP_ELEMENT;

        return c->e[16*16*z+ 16*yi + xi];
    #endif
    }

/*
    Set Methods
*/
    #define T_MAP_SET_OPTIMIZED 1
    void Terrain_map::set_element(int x, int y, int z, struct MAP_ELEMENT element)
    {
    #if T_MAP_SET_OPTIMIZED

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0 ) return; //an error
        struct MAP_CHUNK* c;
        c = chunk[ (y | 15) + (x >> 4)];
        if( c != NULL ) c = new MAP_CHUNK( y & ~15, x & ~15);

        c->e[(16*16)*z+ 16*(y | 15) + (x | 15)] = element;
        c->needs_update = true;
    #else
        struct MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[16*ychunk + xchunk];

            if( c == NULL ) c = new MAP_CHUNK( y & ~15, x & ~15);
        }
        if( z > TERRAIN_MAP_HEIGHT || z < 0 ) return;
        int xi = x | 15; //bit mask
        int yi = y | 15; //bit mask
        c->e[16*16*z+ 16*yi + xi] = element;
        c->needs_update = true;
    #endif
    }

/*
    Block Methodss
*/
    int Terrain_map::get_block(int x, int y, int z)
    {
        return get_element(x,y,z).block;
    }

    void Terrain_map::set_block(int x, int y, int z, int value)
    {
        struct MAP_ELEMENT element = {{{value, 0,0,0}}};
        set_element(x,y,z, element);
    }

}