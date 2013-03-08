#include "t_map.hpp"

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/t_map_class.hpp>
#include <t_map/glsl/texture.hpp>
#include <t_map/lighting.hpp>
#include <physics/vec3.hpp>
#include <common/defines.h>
#include <t_map/common/types.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/map_element.hpp>

#if DC_CLIENT
# include <t_map/glsl/cache.hpp>
# include <t_map/glsl/shader.hpp>
# include <t_map/net/t_StoC.hpp>
#endif

#if DC_SERVER
# include <common/random.hpp>
# include <t_map/server/manager.hpp>
# include <t_map/server/subscription_list.hpp>
# include <t_map/server/env_process.hpp>
# include <particle/_interface.hpp>
# include <item/_interface.hpp>
# include <entity/object/main.hpp>
# include <t_map/config/drop_dat.hpp>
# include <t_gen/explosives.hpp>
#endif

namespace t_map
{

class Terrain_map* main_map = NULL;

CubeType get(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return (CubeType)ABOVE_MAP_ELEMENT.block;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    class MapChunk* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if (c == NULL)
        return (CubeType)NULL_MAP_ELEMENT.block;
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
    
    //main_map->radiation_block_list.create(x,y,z);


    if (isItemContainer(existing_cube_type))
        destroy_item_container_block(x,y,z);
    #endif

    main_map->set_block(x,y,z, cube_type);
    light_add_block(x,y,z);
    return cube_type;
}

CubeType get(const struct Vec3i& pos)
{
    return get(pos.x, pos.y, pos.z);
}

CubeType set(const struct Vec3i& pos, CubeType cube_type)
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

}   // t_map
