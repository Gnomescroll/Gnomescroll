#include "utils.hpp"

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/t_map.hpp>

namespace t_map
{

Vec3i random_surface_block()
{
    Vec3i p;
    p.x = randrange(0, map_dim.x - 1);
    p.y = randrange(0, map_dim.y - 1);
    p.z = t_map::get_highest_solid_block(p.x, p.y);
    return p;
}

inline int get_highest_open_block(int x, int y, int n)
{
    IF_ASSERT(n < 1) return -1;
    if (n == 1) return get_highest_open_block(x, y);

    int open = n;
    CubeType cube_type;
    int i;

    for (i=map_dim.z; i>=0; i--)
    {
        cube_type = get(x, y, i);
        if (!isSolid(cube_type))
            open++;
        else
        {
            if (open >= n) return i+1;
            open = 0;
        }
    }
    if (open >= n) return 0;
    return -1;
}

inline int get_highest_open_block(const Vec3i& position, int clearance)
{
    return get_highest_open_block(position.x, position.y, clearance);
}

inline int get_highest_open_block(int x, int y)
{
    return get_highest_solid_block(x, y) + 1;
}

inline int get_highest_open_block(const Vec3i& position)
{
    return get_highest_open_block(position.x, position.y);
}

inline bool _can_fit(int x, int y, int z, int clearance)
{
    int cursor = 0;
    while (cursor < clearance && !isSolid(x, y, z + cursor))
        cursor++;
    return (cursor == clearance);
}

inline int _can_fit(const Vec3i& position, int clearance)
{
    return _can_fit(position.x, position.y, position.z, clearance);
}


inline int _get_next_down(int x, int y, int z, int clearance)
{
    if (z <= 0) return -1;
    do
    {
        while (isSolid(x, y, z)) z--;
        while (z >= 0 && !isSolid(x, y, z)) z--;
    } while (z >= 0 && !_can_fit(x, y, z+1, clearance));
    return z+1;
}

inline int _get_next_down(const Vec3i& position, int clearance)
{
    return _get_next_down(position.x, position.y, position.z, clearance);
}

inline int _get_next_up(int x, int y, int z, int clearance)
{
    if (z >= map_dim.z)
        return map_dim.z + 1;
    while (isSolid(x, y, z)) z++;
    while (z <= map_dim.z && !_can_fit(x, y, z, clearance))
    {
        while (z <= map_dim.z && !isSolid(x, y, z))
            z++;
        while (isSolid(x, y, z)) z++;
    }
    return z;
}

inline int _get_next_up(const Vec3i& position, int clearance)
{
    return _get_next_up(position.x, position.y, position.z, clearance);
}

inline size_t get_nearest_surface_blocks(const struct Vec3i& pos, int clearance, int* out, size_t outlen)
{   // the block will also be a surface block, if found
    // TODO -- this does not return them in strictly nearest order
    // it returns them alternating up,down,up,down (whichever is closer of the two)
    // until it runs out in either direction then it fills with the remaining of the other
    // It should return it in strictly nearest order
    IF_ASSERT(outlen < 1) return 0;
    size_t i = 0;
    IF_ASSERT(clearance < 1)
    {
        out[i++] = pos.z;
        return i;
    }

    int down = _get_next_down(pos.x, pos.y, pos.z, clearance);
    int up = pos.z + 1;
    if (!isSolid(pos))
        while (up <= map_dim.z && !isSolid(pos.x, pos.y, up))
            up++;
    up = _get_next_up(pos.x, pos.y, up, clearance);

    int n = 0;
    int ret[2] = {0};
    while (down >= 0 || up <= map_dim.z)
    {
        n = 0;
        if (down < 0 || up - pos.z < pos.z - down)
        {   // the up position is closer
            if (up <= map_dim.z)
                ret[n++] = up;
            if (down >= 0)
                ret[n++] = down;
        }
        else
        {
            if (down >= 0)
                ret[n++] = down;
            if (up <= map_dim.z)
                ret[n++] = up;
        }

        for (int k=0; k<n && i<outlen; k++, i++)
            out[i] = ret[k];
        if (i >= outlen)
            return i;

        while (up <= map_dim.z && !isSolid(pos.x, pos.y, up))
            up++;
        up = _get_next_up(pos.x, pos.y, up, clearance);
        down = _get_next_down(pos.x, pos.y, down - 1, clearance);
    }

    return i;
}

inline int get_nearest_surface_block(const struct Vec3i& pos, int clearance)
{
    int out[1] = {0};
    get_nearest_surface_blocks(pos, clearance, out, 1);
    return out[0];
}

inline int get_nearest_surface_block(const struct Vec3i& pos)
{
    return get_nearest_surface_block(pos, 1);
}

inline int get_nearest_surface_block(int x, int y, int z, int clearance)
{
    return get_nearest_surface_block(vec3i_init(x, y, z), clearance);
}

inline int get_nearest_surface_block(int x, int y, int z)
{
    return get_nearest_surface_block(vec3i_init(x, y, z));
}

inline int get_nearest_surface_block_below(const struct Vec3i& pos, int clearance, int max)
{   // a return value is ==0 is failure
    int z = pos.z;
    int quit = z - max;
    while (z >= quit && isSolid(pos.x, pos.y, z))
        z--;
    if (z < quit) return 0;
    while (z >= quit && z >= 0 && !isSolid(pos.x, pos.y, z))
        z--;
    if (z < quit) return 0;
    z++;
    return z;
}

inline int get_nearest_surface_block_above(const struct Vec3i& pos, int clearance, int max)
{   // a return value of > map_dim.z is failure -- no surface blocks here
    int z = pos.z + 1;
    int quit = z + max;
    if (!isSolid(pos.x, pos.y, z))
    {
        if (isSolid(pos.x, pos.y, z-1))
            return z;
        while (z <= quit && z <= map_dim.z && !isSolid(pos.x, pos.y, z))
            z++;
        if (z > quit) return map_dim.z + 1;
    }
    while (z <= quit && isSolid(pos.x, pos.y, z))
        z++;
    if (z > quit) return map_dim.z + 1;
    return z;
}

inline int get_nearest_surface_block_below(const struct Vec3i& pos, int clearance)
{
    return get_nearest_surface_block_below(pos, clearance, map_dim.z);
}

inline int get_nearest_surface_block_above(const struct Vec3i& pos, int clearance)
{
    return get_nearest_surface_block_above(pos, clearance, map_dim.z);
}

inline int get_nearest_surface_block_below(const struct Vec3i& pos)
{
    return get_nearest_surface_block_below(pos, 1);
}

inline int get_nearest_surface_block_above(const struct Vec3i& pos)
{
    return get_nearest_surface_block_above(pos, 1);
}


inline bool is_surface_block(const Vec3i& position)
{   // returns true if the block is not solid and the block underneath is solid
    return (t_map::get(position) == EMPTY_CUBE &&
            t_map::get(position.x, position.y, position.z - 1) != EMPTY_CUBE);
}

inline bool is_surface_block(int x, int y, int z)
{
    return is_surface_block(vec3i_init(x, y, z));
}

inline int get_solid_block_below(const struct Vec3i& p)
{
    for (int i=p.z-1; i>=0; i--)
        if (isSolid(p.x, p.y, i))
            return i;
    return -1;
}

inline int get_solid_block_below(int x, int y, int z)
{
    return get_solid_block_below(vec3i_init(x, y, z));
}

inline int get_open_block_below(const struct Vec3i& p)
{
    int z = get_solid_block_below(p);
    if (z < 0) return -1;
    return z;
}

inline int get_open_block_below(int x, int y, int z)
{
    return get_open_block_below(vec3i_init(x, y, z));
}

inline int get_highest_solid_block(const Vec3i& position)
{
    // TODO -- use heightmap cache
    int i = position.z - 1;
    for (; i>=0; i--)
        if (isSolid(position.x, position.y, i)) break;
    return i;
}

inline int get_highest_solid_block(int x, int y)
{
    return get_highest_solid_block(vec3i_init(x, y, map_dim.z));
}

inline int get_lowest_open_block(int x, int y, int n)
{
    IF_ASSERT(n < 1) return -1;
    int open = 0;
    int i = 0;
    for (; i<=map_dim.z; i++)
    {
        CubeType cube_type = get(x, y, i);
        if (isSolid(cube_type)) open = 0;
        else open++;
        if (open >= n) return (i - open + 1);
    }
    return i;
}

inline int get_lowest_solid_block(int x, int y)
{
    int i = 0;
    for (; i<map_dim.z; i++)
        if (isSolid(x,y,i)) break;
    if (i >= map_dim.z) i = -1;  // failure
    return i;
}

inline bool position_is_loaded(int x, int y)
{
    #if DC_SERVER
    return true;
    #endif

    #if DC_CLIENT
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    int cx = x / TERRAIN_CHUNK_WIDTH;
    int cy = y / TERRAIN_CHUNK_WIDTH;
    int index = main_map->xchunk_dim*cy+cx;

    //if map chunk is null, it is not loaded
    return (main_map->chunk[index] != NULL);
    #endif
}

bool block_can_be_placed(const Vec3i& position, CubeType cube_type)
{
    IF_ASSERT(!is_valid_z(position)) return false;
    Vec3i p = translate_position(position);

    IF_ASSERT(!isValidCube(cube_type)) return false;

    if (get(p) != EMPTY_CUBE) return false;
    // check against all spawners
    if (Entities::point_occupied_by_type(ENTITY_AGENT_SPAWNER, p))
        return false;
    if (Entities::point_occupied_by_type(ENTITY_ENERGY_CORE, p))
        return false;
    return true;
}


}   // t_map
