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
# include <entity/entity/main.hpp>
# include <t_map/config/drop_dat.hpp>
# include <t_gen/explosives.hpp>
#endif

namespace t_map
{

class TerrainMap* main_map = NULL;

CubeType get(const Vec3i& position)
{
    if (!is_valid_z(position))
        return (CubeType)ABOVE_MAP_ELEMENT.block;
    Vec3i p = translate_position(position);
    class MapChunk* c = main_map->get_chunk(p);
    if (c == NULL)
        return (CubeType)NULL_MAP_ELEMENT.block;
    else
        return c->get_block(p);
}

CubeType set(const Vec3i& position, CubeType cube_type)
{
    main_map->set_block(position, cube_type);
    light_add_block(position);
    return cube_type;
}

CubeType set(const Vec3i& position, const char* cube)
{
    CubeType cube_type = get_cube_type(cube);
    IF_ASSERT(!isValid(cube_type)) return cube_type;
    return set(position, cube_type);
}

CubeType get(int x, int y, int z)
{
    return get(vec3i_init(x, y, z));
}

CubeType set(int x, int y, int z, CubeType cube_type)
{
    return set(vec3i_init(x, y, z), cube_type);
}

OPTIMIZED ALWAYS_INLINE
void set_fast(const Vec3i& position, CubeType cube_type)
{
    main_map->set_block_fast(position, cube_type);
}

OPTIMIZED ALWAYS_INLINE
void set_fast(int x, int y, int z, CubeType cube_type)
{
    return set_fast(vec3i_init(x, y, z), cube_type);
}

struct MapElement get_element(const Vec3i& position)
{
    if (!is_valid_z(position)) return ABOVE_MAP_ELEMENT;
    Vec3i p = translate_position(position);
    class MapChunk* c = main_map->get_chunk(p);
    if (c == NULL)
        return NULL_MAP_ELEMENT;
    else
        return c->get_element(p);
}

inline struct MapElement get_element(int x, int y, int z)
{
    return get_element(vec3i_init(x, y, z));
}

inline void set_element(const Vec3i& position, struct MapElement e)
{
    main_map->set_element(position, e);
    light_add_block(position);
}

inline void set_element(int x, int y, int z, struct MapElement e)
{
    set_element(vec3i_init(x, y, z), e);
}

void set_palette(const Vec3i& position, int palette)
{
    struct MapElement element = main_map->get_element(position);
    element.palette = palette;
    main_map->set_element(position, element);
}

int get_palette(const Vec3i& position)
{
    return main_map->get_element(position).palette;
}

class TerrainMap* get_map()
{
    return main_map;
}

void init_t_map()
{
    GS_ASSERT(main_map == NULL);
    main_map = new TerrainMap(map_dim.x, map_dim.y);

    init_textures();

    init_lighting();

    #if DC_CLIENT
    init_client_compressors();
    init_t_vbo();
    #endif

    #if DC_SERVER
    GS_ASSERT(map_history == NULL);
    map_history = new TerrainMapSubscription(map_dim.x, map_dim.y);
    init_env_process();
    #endif
}

#if DC_CLIENT
void init_for_draw()
{
    init_cache();
    //control_node_render_init(&main_map->control_node_list);
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

int get_block_damage(const Vec3i& position)
{
    IF_ASSERT(!is_valid_z(position)) return 0;
    return main_map->get_damage(translate_position(position));
}

#if DC_SERVER

// apply block damage & broadcast the update to client
void apply_damage_broadcast(const Vec3i& position, int dmg, TerrainModificationAction action)
{
    IF_ASSERT(dmg <= 0) return;
    IF_ASSERT(!is_valid_z(position)) return;
    if (position.z == 0) return;    // no floor
    Vec3i p = translate_position(position);

    CubeType cube_type = ERROR_CUBE;
    int ret = t_map::main_map->apply_damage(p, dmg, &cube_type);
    if (ret != 0) return;

    set(p, EMPTY_CUBE);  //clear block
    t_mech::handle_block_removal(p);    //block removal

    // always explode explosives with force
    if (isExplosive(cube_type))
    {
        handle_explosive_block(p);
        action = TMA_PLASMAGEN;
    }

    // block_action packet expects final value of cube, not initial value
    map_history->send_block_action(p, EMPTY_CUBE, action);

    if (hasItemDrop(cube_type))
        handle_block_drop(p, cube_type);
}

void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action)
{
    return apply_damage_broadcast(vec3i_init(x, y, z), dmg, action);
}

void broadcast_set_block_action(const Vec3i& position, CubeType cube_type, int action)
{
    IF_ASSERT(!is_valid_z(position)) return;
    Vec3i p = translate_position(position);
    map_history->send_block_action(p, cube_type, action);
}

bool broadcast_set_block(const Vec3i& position, CubeType cube_type)
{
    IF_ASSERT(!is_valid_z(position)) return false;
    Vec3i p = translate_position(position);

    set(p, cube_type);
    map_history->send_set_block(p, cube_type);
    return true;
}

bool broadcast_set_block(const Vec3i& position, const char* cube)
{
    CubeType cube_type = get_cube_type(cube);
    IF_ASSERT(!isValid(cube_type)) return cube_type;
    return broadcast_set_block(position, cube_type);
}

void broadcast_set_block_palette(const Vec3i& position, CubeType block, int palette)
{
    IF_ASSERT(!is_valid_z(position)) return;
    Vec3i p = translate_position(position);

    struct MapElement e = NULL_MAP_ELEMENT;
    e.block = block;
    e.palette = palette;

    set_element(p,e);
    map_history->send_set_block_palette(p, block,palette);
}

void broadcast_set_block_palette(int x, int y, int z, CubeType block, int palette)
{
    broadcast_set_block_palette(vec3i_init(x, y, z), block, palette);
}

void broadcast_set_palette(const Vec3i& position, int palette)
{
    IF_ASSERT(!is_valid_z(position)) return;
    Vec3i p = translate_position(position);

    struct MapElement e = get_element(p);
    e.palette = palette;

    set_element(p,e);
    map_history->send_set_block_palette(p, (CubeType)e.block, e.palette);
}
#endif


int get_rad_level(const struct Vec3i& p)
{
    return main_map->radiation_block_list.get_rad_level(p);
}

}   // t_map
