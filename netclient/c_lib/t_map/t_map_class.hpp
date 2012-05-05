#pragma once

#include <t_map/constants.hpp>

namespace t_map
{

#include <t_map/common/map_element.hpp>

/*
    Optimization parameters
*/
#define TERRAIN_MAP_FORCE_INLINE 0 //inlining may slow program from cache misses

#define T_MAP_GET_DEBUG 0  //disable for more speed
#define T_MAP_GET_OPTIMIZED 1

#define T_MAP_SET_OPTIMIZED 0   //not safe
//#define T_MAP_SET_DEBUG 1

/*
    Implementation
*/

class MAP_CHUNK
{
    public:

    int xpos;
    int ypos;

    #ifdef DC_CLIENT
    bool needs_update;
    #endif

    unsigned char top_block[MAP_CHUNK_WIDTH*MAP_CHUNK_HEIGHT];

    struct MAP_ELEMENT e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT];

    MAP_CHUNK(int _xpos, int _ypos);

    inline int get_block(int x, int y, int z); __attribute((always_inline)); //for VBO generator
};

class Terrain_map
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_CHUNK** chunk;

    #if DC_CLIENT
    bool height_changed;
    bool chunk_heights_changed[MAP_CHUNK_WIDTH*MAP_CHUNK_HEIGHT];
    void reset_heights_read();
    void chunk_received(int cx, int cy);    // callback, used by decompressed chunk msg handler
    inline unsigned char get_cached_height(int x, int y);
    void update_heights(int x, int y, int z, int val);
    unsigned char column_heights[MAP_WIDTH * MAP_HEIGHT];               // 1x1 columns
    #endif
    
    Terrain_map(int _xdim, int _ydim);
    ~Terrain_map();

#if DC_CLIENT
    void set_update(int x, int y);
#endif

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
    int apply_damage(int x, int y, int z, int dmg, int* block_type);


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
