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
    class MapChunk* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
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

CubeType get(const struct MapPos& pos)
{
    return get(pos.x, pos.y, pos.z);
}

CubeType set(const struct MapPos& pos, CubeType cube_type)
{
    return set(pos.x, pos.y, pos.z, cube_type);
}

OPTIMIZED ALWAYS_INLINE
void set_fast(int x, int y, int z, CubeType cube_type)
{
    main_map->set_block_fast(x,y,z, cube_type);
}

struct MapElement get_element(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return ABOVE_MAP_ELEMENT;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MapChunk* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if (c == NULL) return NULL_MAP_ELEMENT;
    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];
}

void set_element(int x, int y, int z, struct MapElement e)
{
    main_map->set_element(x,y,z,e);
    light_add_block(x,y,z);
}

void set_palette(int x, int y, int z, int palette)
{
    struct MapElement element = main_map->get_element(x,y,z);
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

    struct MapElement e = NULL_MAP_ELEMENT;
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

    struct MapElement e = get_element(x,y,z);
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

inline bool _can_fit(int x, int y, int z, int clearance)
{
    int cursor = 0;
    while (cursor < clearance && !isSolid(x, y, z + cursor))
        cursor++;
    return (cursor == clearance);
}

inline void GetNearestSurfaceBlockIter::init()
{
    this->up_done = false;
    this->down_done = false;
    IF_ASSERT(clearance < 1) this->clearance = 1;
    this->down = _get_next_down(this->pos.x, this->pos.y, this->pos.z);
    this->up = this->pos.z + 1;
    if (t_map::get(this->pos) == EMPTY_CUBE)
        while (this->up <= map_dim.z &&
               !isSolid(this->pos.x, this->pos.y, this->up))
            this->up++;
    this->up = _get_next_up(this->pos.x, this->pos.y, this->up);
}

int GetNearestSurfaceBlockIter::next()
{   // the block will also be a surface block, if found
    while ((this->down >= 0 || this->up <= map_dim.z) &&
           (!this->up_done || !this->down_done))
    {
        // TODO --
        // function keep reentering and exiting at 128 before it hits the "done" check
        // we need to mark that case as done earlier
        int first = this->down;
        int second = this->up;
        if ((this->down_done || this->down < 0) ||
            (!this->up_done && this->up - this->pos.z < this->pos.z - down))
        {   // the up position is closer
            first = this->up;
            second = this->down;
        }
        if (_can_fit(this->pos.x, this->pos.y, first, this->clearance))
            return first;
        else
        if (_can_fit(this->pos.x, this->pos.y, second, this->clearance))
            return second;
        while (this->up <= map_dim.z &&
               !isSolid(this->pos.x, this->pos.y, this->up))
            this->up++;
        int next_up = _get_next_up(this->pos.x, this->pos.y, this->up);
        int next_down = _get_next_down(this->pos.x, this->pos.y,
                                       this->down - 1);
        printf("Next up: %d, up: %d. Next down: %d, down: %d\n", next_up, this->up, next_down, this->down);
        this->up_done = (next_up == this->up);
        this->down_done = (next_down == this->down);
        this->up = next_up;
        this->down = next_down;
    }
    return this->END;
}

static GetNearestSurfaceBlockIter _get_nearest_surface_block_iter;

inline int get_nearest_surface_block(const struct MapPos& pos, int clearance)
{
    _get_nearest_surface_block_iter.init(pos, clearance);
    return _get_nearest_surface_block_iter.next();
}

inline int get_nearest_surface_block(const struct MapPos& pos)
{
    return get_nearest_surface_block(pos, 1);
}

inline int get_nearest_surface_block(int x, int y, int z, int clearance)
{
    struct MapPos pos = map_pos_init(x, y, z);
    return get_nearest_surface_block(pos, clearance);
}

inline int get_nearest_surface_block(int x, int y, int z)
{
    struct MapPos pos = map_pos_init(x, y, z);
    return get_nearest_surface_block(pos);
}

inline bool is_surface_block(int x, int y, int z)
{   // returns true if the block is not solid and the block underneath is solid
    return (t_map::get(x, y, z) == EMPTY_CUBE &&
            t_map::get(x, y, z-1) != EMPTY_CUBE);
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
