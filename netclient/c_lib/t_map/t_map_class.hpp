#pragma once

namespace t_map
{

#include <c_lib/t_map/common/map_element.hpp>
    
/*
    Optimization parameters
*/
#define TERRAIN_MAP_FORCE_INLINE 0 //inlining may slow program from cache misses

#define T_MAP_GET_DEBUG 0  //disable for more speed
#define T_MAP_GET_OPTIMIZED 1

/*
    Implementation
*/
const int TERRAIN_MAP_HEIGHT = 128;


class MAP_CHUNK
{
    public:

    int xpos;
    int ypos;

    #ifdef DC_CLIENT
        bool needs_update;
    #endif

    unsigned char top_block[16*16];

    struct MAP_ELEMENT e[16*16*TERRAIN_MAP_HEIGHT];

    MAP_CHUNK(int _xpos, int _ypos);

};

class Terrain_map
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_CHUNK** chunk;

    Terrain_map(int _xdim, int _ydim);
    ~Terrain_map();
    
#if TERRAIN_MAP_FORCE_INLINE
    inline struct MAP_ELEMENT get_element(int x, int y, int z) __attribute((always_inline));
    inline void set_element(int x, int y, int z, struct MAP_ELEMENT element) __attribute((always_inline));

    inline int get_block(int x, int y, int z) __attribute((always_inline));
    inline void set_block(int x, int y, int z, int value) __attribute((always_inline));
#else
    struct MAP_ELEMENT get_element(int x, int y, int z);
    void set_element(int x, int y, int z, struct MAP_ELEMENT element);
    int get_block(int x, int y, int z);
    void set_block(int x, int y, int z, int value);
#endif


    int apply_damage(int x, int y, int z, int dmg);

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
