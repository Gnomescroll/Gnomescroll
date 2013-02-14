#include "t_map.hpp"

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/t_map_class.hpp>
#include <t_map/glsl/texture.hpp>
#include <t_map/lighting.hpp>

#if DC_CLIENT
# include <t_map/glsl/cache.hpp>
# include <t_map/glsl/shader.hpp>
# include <t_map/net/t_StoC.hpp>
#endif

#if DC_SERVER
# include <t_map/server/manager.hpp>
# include <t_map/server/subscription_list.hpp>
# include <t_map/server/env_process.hpp>
# include <common/random.hpp>
# include <particle/_interface.hpp>
# include <item/_interface.hpp>
# include <t_map/config/drop_dat.hpp>
# include <entity/object/main.hpp>
# include <t_gen/explosives.hpp>
#endif

namespace t_map
{

class Terrain_map* main_map = NULL;

CubeType get(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return EMPTY_CUBE;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if (c == NULL) return EMPTY_CUBE;
    return (CubeType)c->e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
}

CubeType set(int x, int y, int z, CubeType cube_type)
{
    #if DC_SERVER
    CubeType existing_cube_type = t_map::get(x,y,z);
    // dont overwrite existing cubes with new ones
    //IF_ASSERT(cube_type != EMPTY_CUBE && existing_cube_type != EMPTY_CUBE) return existing_cube_type;

    // TODO -- make a single handler for this item container check,
    // and the explosives checks (any special destruction required blocks)
    // and call it here (along with the apply_damage location)
    if (isItemContainer(existing_cube_type))
        destroy_item_container_block(x,y,z);
    #endif

    main_map->set_block(x,y,z, cube_type);
    light_add_block(x,y,z);
    return cube_type;
}

OPTIMIZED ALWAYS_INLINE
void set_fast(int x, int y, int z, CubeType cube_type)
{
    main_map->set_block_fast(x,y,z, cube_type);
}

struct MAP_ELEMENT get_element(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return ABOVE_MAP_ELEMENT;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if (c == NULL) return NULL_MAP_ELEMENT;
    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];
}

void set_element(int x, int y, int z, struct MAP_ELEMENT e)
{
    main_map->set_element(x,y,z,e);
    light_add_block(x,y,z);
}

void set_palette(int x, int y, int z, int palette)
{
    struct MAP_ELEMENT element = main_map->get_element(x,y,z);
    element.palette = palette;
    main_map->set_element(x,y,z,element);
}

int get_palette(int x, int y, int z)
{
    return main_map->get_element(x,y,z).palette;
}

class Terrain_map* get_map()
{
    return main_map;
}

void init_t_map()
{
    GS_ASSERT(main_map == NULL);
    main_map = new Terrain_map(MAP_WIDTH, MAP_HEIGHT); //512 by 512 map

    init_textures();

    init_lighting();

    #if DC_CLIENT
    init_client_compressors();
    init_t_vbo();
    #endif

    #if DC_SERVER
    GS_ASSERT(map_history == NULL);
    map_history = new Terrain_map_subscription(MAP_WIDTH, MAP_HEIGHT);
    init_env_process();
    #endif
}

#if DC_CLIENT
void init_for_draw()
{
    init_cache();
    control_node_render_init(&main_map->control_node_list);
    init_shaders();
}
#endif

void end_t_map()
{
    delete main_map;

    #if DC_CLIENT
    end_client_compressors();
    teardown_t_vbo();
    control_node_render_teardown();
    #endif

    #if DC_SERVER
    delete map_history;
    teardown_env_process();
    #endif

    teardown_textures();
    teardown_lighting();

}

int get_block_damage(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return 0;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    return main_map->get_damage(x,y,z);
}

#if DC_SERVER

// apply block damage & broadcast the update to client
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action)
{
    IF_ASSERT(dmg <= 0) return;
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    CubeType cube_type = ERROR_CUBE;
    int ret = t_map::main_map->apply_damage(x,y,z, dmg, &cube_type);
    if (ret != 0) return;

    set(x,y,z, EMPTY_CUBE);  //clear block
    t_mech::handle_block_removal(x,y,z);    //block removal

    // always explode explosives with force
    if (isExplosive(cube_type)) action = TMA_PLASMAGEN;

    // block_action packet expects final value of cube, not initial value
    map_history->send_block_action(x,y,z, EMPTY_CUBE, action);

    if (hasItemDrop(cube_type))
        handle_block_drop(x,y,z, cube_type);
}

void broadcast_set_block_action(int x, int y, int z, CubeType cube_type, int action)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    map_history->send_block_action(x,y,z, cube_type, action);
}

bool broadcast_set_block(int x, int y, int z, CubeType cube_type)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return false;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    set(x,y,z, cube_type);
    map_history->send_set_block(x,y,z, cube_type);
    return true;
}

void broadcast_set_block_palette(int x, int y, int z, CubeType block, int palette)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    struct MAP_ELEMENT e = NULL_MAP_ELEMENT;
    e.block = block;
    e.palette = palette;

    set_element(x,y,z,e);
    map_history->send_set_block_palette(x,y,z, block,palette);
}

void broadcast_set_palette(int x, int y, int z, int palette)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    struct MAP_ELEMENT e = get_element(x,y,z);
    e.palette = palette;

    set_element(x,y,z,e);
    map_history->send_set_block_palette(x,y,z, (CubeType)e.block, e.palette);
}

#endif

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

inline int get_highest_open_block(int x, int y)
{
    return get_highest_solid_block(x, y) + 1;
}

inline int _get_next_down(int x, int y, int z)
{
    if (z <= 0) return -1;
    while (isSolid(x, y, z))
        z--;
    while (z >= 0 && !isSolid(x, y, z))
        z--;
    return z + 1;
}

inline int _get_next_up(int x, int y, int z)
{
    if (z >= map_dim.z) return map_dim.z + 1;
    while (isSolid(x, y, z))
        z++;
    return z;
}

inline bool _can_fit(int x, int y, int z, int n)
{
    int cursor = 0;
    while (cursor < n && !isSolid(x, y, z + cursor))
        cursor++;
    return (cursor == n);
}

inline int get_nearest_open_block(int x, int y, int z, int n)
{   // the block will also be a surface block, if found
    IF_ASSERT(n < 1) return z;
    int down = _get_next_down(x, y, z);
    int up = z + 1;
    if (get(x, y, z) == EMPTY_CUBE)
        while (up <= map_dim.z && !isSolid(x, y, up))
            up++;
    up = _get_next_up(x, y, up);
    while (down >= 0 || up <= map_dim.z)
    {
        int first = down;
        int second = up;
        if (down < 0 || up - z < z - down)
        {   // the up position is closer
            first = up;
            second = down;
        }
        if (_can_fit(x, y, first, n)) return first;
        else if (_can_fit(x, y, second, n)) return second;
        while (up <= map_dim.z && !isSolid(x, y, up))
            up++;
        up = _get_next_up(x, y, up);
        down = _get_next_down(x, y, down - 1);
    }
    return 0;
}

inline int get_nearest_open_block(int x, int y, int z)
{
    return get_nearest_open_block(x, y, z, 1);
}

inline bool is_surface_block(int x, int y, int z)
{
    return (t_map::get(x, y, z) == EMPTY_CUBE &&
            t_map::get(x, y, z-1) != EMPTY_CUBE);
}

inline int get_nearest_surface_block(int x, int y, int z)
{   // TODO -- can be optimized to reduce t_map::get calls
    int upper = z + 1;
    int lower = z;
    while (lower > 0 || upper <= map_dim.z)
    {
        if (is_surface_block(x, y, lower)) return lower;
        if (is_surface_block(x, y, upper)) return upper;
        lower--;
        upper++;
    }
    return map_dim.z;
}

inline int get_solid_block_below(int x, int y, int z)
{
    for (int i=z-1; i>=0; i--)
        if (isSolid(x, y, i))
            return i;
    return -1;
}

inline int get_open_block_below(int x, int y, int z)
{
    z = get_solid_block_below(x, y, z);
    if (z < 0) return -1;
    return z;
}

inline int get_highest_solid_block(int x, int y)
{
    return get_highest_solid_block(x,y, map_dim.z);
}

inline int get_highest_solid_block(int x, int y, int z)
{
    // TODO -- use heightmap cache
    int i = z - 1;
    for (; i>=0; i--)
        if (isSolid(x,y,i)) break;
    return i;
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

bool block_can_be_placed(int x, int y, int z, CubeType cube_type)
{
    IF_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return false;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    IF_ASSERT(!isValidCube(cube_type)) return false;

    if (get(x,y,z) != EMPTY_CUBE) return false;
    // check against all spawners
    if (Entities::point_occupied_by_type(OBJECT_AGENT_SPAWNER, x,y,z))
        return false;
    if (Entities::point_occupied_by_type(OBJECT_ENERGY_CORE, x,y,z))
        return false;
    return true;
}

}   // t_map
