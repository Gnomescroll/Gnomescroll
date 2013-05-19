#pragma once

#include <t_map/common/constants.hpp>
#include <t_map/chunk_special.hpp>
#include <t_map/common/map_element.hpp>
#include <t_map/map_special.hpp>

namespace t_map
{

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

class MapChunk
{
    public:
        int chunk_index;
        int xpos;
        int ypos;
        class ChunkItemContainer chunk_item_container;
        unsigned char height_cache[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH];
        int version; //increment on map changes
        struct MapElement e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT];
        #if DC_CLIENT
        bool needs_update;
        #endif

    MapChunk(int _xpos, int _ypos);

    ALWAYS_INLINE CubeType get_block(const Vec3i& position);
    ALWAYS_INLINE CubeType get_block(int x, int y, int z);
    ALWAYS_INLINE struct MapElement get_element(const Vec3i& position);
    ALWAYS_INLINE struct MapElement get_element(int x, int y, int z);

    ALWAYS_INLINE void set_element(const Vec3i& position, struct MapElement element)
    {
        return this->set_element(position.x, position.y, position.z, element);
    }

    ALWAYS_INLINE void set_element(int x, int y, int z, struct MapElement element)
    {
        e[this->get_element_index(x, y, z)] = element;
    }

    ALWAYS_INLINE int get_element_index(const struct Vec3i& position)
    {
        return this->get_element_index(position.x, position.y, position.z);
    }

    ALWAYS_INLINE int get_element_index(int x, int y, int z)
    {
        return (z << 8) + ((y & 15) << 4) + (x & 15);
    }

    void refresh_height_cache();

    void increment_version()
    {
        this->version++;
    }
};

class Terrain_map
{
    public:

        int xdim;
        int ydim;

        static const int xchunk_dim = MAP_CHUNK_XDIM;
        static const int ychunk_dim = MAP_CHUNK_XDIM;

        class MapChunk** chunk;

        class ControlNodeList control_node_list; //special
        class RadiationBlockList radiation_block_list; //special

    #if DC_CLIENT
        bool height_changed;
        char chunk_heights_status[MAP_CHUNK_XDIM*MAP_CHUNK_YDIM];   // status of column height (set, unchanged, changed)
        unsigned char* column_heights;                              // 1x1 columns

    void reset_heights_read();
    void chunk_received(int cx, int cy);    // callback, used by decompressed chunk msg handler
    inline unsigned char get_cached_height(int x, int y);
    void update_heights(const Vec3i& position, CubeType cube_type);
    void set_update(int x, int y);
    bool chunk_loaded(const Vec3i& position); //checks if chunk is non null
    #endif

    Terrain_map(int _xdim, int _ydim);
    ~Terrain_map();

    void load_chunk(int i, int j);      //only entry point for loading
    void unload_chunk(int i, int j);    //only entry point for unloading

    #if TERRAIN_MAP_FORCE_INLINE
    # define T_MAP_INLINE ALWAYS_INLINE
    #else
    # define T_MAP_INLINE
    #endif
    T_MAP_INLINE struct MapElement get_element(const Vec3i& position);
    T_MAP_INLINE struct MapElement get_element(int x, int y, int z);
    T_MAP_INLINE void set_element(const Vec3i& position, struct MapElement element);
    T_MAP_INLINE CubeType get_block(const Vec3i& position);
    T_MAP_INLINE void set_block(const Vec3i& position, CubeType cube_type);
    #undef T_MAP_INLINE

    ALWAYS_INLINE void set_block_fast(const Vec3i& position, CubeType cube_type);

    ALWAYS_INLINE int get_chunk_index(const Vec3i& position)
    {
        return (MAP_CHUNK_XDIM * (position.y >> 4)) + (position.x >> 4);
    }

    ALWAYS_INLINE MapChunk* get_chunk(const Vec3i& position)
    {
        return this->chunk[this->get_chunk_index(position)];
    }

    int get_damage(const Vec3i& position);
    int apply_damage(const Vec3i& position, int dmg, CubeType* cube_type);
    inline int get_height(int x, int y);
};

} // t_map

//too complicated for now

/*
struct ChunkSlice
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
*/
