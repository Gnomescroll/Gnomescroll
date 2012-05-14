#include "_interface.hpp"

#if DC_CLIENT
#include <t_map/t_vbo_class.hpp>
#include <t_map/t_vbo_update.hpp>
#include <t_map/t_vbo_draw.hpp>
#endif

#include <t_map/common/constants.hpp>

namespace t_map
{
    
int get_block_item_container(int x, int y, int z)
{
    if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
        | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
    ) GS_ABORT();

    struct MAP_CHUNK* c= main_map->chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];

    if(c == NULL) GS_ABORT();

    //c->chunk_item_container.add(x,y,z, container_type, container_id);

    return NULL_CONTAINER;
}

#if DC_CLIENT
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);

        t_vbo_update_init();

        vbo_draw_init();

    }

    void end_t_vbo()
    {
        delete vbo_map;
        t_vbo_update_end();
        vbo_draw_end();
    }

    void draw_map()
    {
        vbo_map->draw_map();
    }

    void update_map()
    {
        vbo_map->update_map();
    }
#endif

#if DC_SERVER
void create_item_container_block(int x, int y, int z, int container_type, int container_id)
{

    if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
        | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
    ) GS_ABORT();

    struct MAP_CHUNK* c= main_map->chunk[ MAP_CHUNK_WIDTH*(y >> 4) + (x >> 4) ];

    if(c == NULL) GS_ABORT();

    c->chunk_item_container.add(x,y,z, container_type, container_id);

}
#endif

}

