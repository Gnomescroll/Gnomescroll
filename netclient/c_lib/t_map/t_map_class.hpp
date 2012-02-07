#pragma once

namespace t_map
{

struct MAP_ELEMENT
{
    union
    {
        struct
        {
            unsigned char block_type;
            unsigned char palette;
            unsigned char damage;
            unsigned char lighting;
        };
        unsigned int n;
    };
};

const int TERRAIN_MAP_HEIGHT = 128;

class MAP_CHUNK
{
    public:

    int xpos;
    int ypos;
    int zpos;

    unsigned char top_block[16*16];

    struct MAP_ELEMENT e[16*16*TERRAIN_MAP_HEIGHT];

    MAP_CHUNK(int _xpos, int _ypos, int _zpos)
    {
        xpos = _xpos;
        ypos = _ypos;
        zpos = _zpos;
        for(int i=0; i<16*16*TERRAIN_MAP_HEIGHT;i++) e[i].n = 0;
        for(int i=0; i<16*16;i++) top_block[i] = 0;
    }

};

class Terrain_map
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_CHUNK* chunk;

    Terrain_map(int _xdim, int _ydim)
    {
        xdim = (_xdim/16)*16; 
        ydim = (_ydim/16)*16;
        xchunk_dim = _xdim/16; 
        ychunk_dim = _ydim/16;
    }
    
    struct MAP_ELEMENT get_element(int x, int y, int z);
    struct MAP_ELEMENT set_element(int x, int y, int z, value);

    

};

} //

//too complicated for now
struct CHUNKSLICE
{
    union
    {
        struct
        {
            bool changed;
            bool top_changed;
            bool bottom_changed;
            bool north_changed;
            bool south_changed;
            bool west_changed;
            bool east_changed;
            bool reserved1;
        };
        unsigned char flags;
    };
};
