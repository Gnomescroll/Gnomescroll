#include "_interface.hpp"

#if DC_CLIENT
#include <t_map/t_vbo_class.hpp>
#include <t_map/t_vbo_update.hpp>
#include <t_map/t_vbo_draw.hpp>
#endif

#include <t_map/common/constants.hpp>

#include <item/common/constant.hpp>
#include <item/common/enum.hpp>

namespace t_map
{
    
int get_block_item_container(int x, int y, int z)
{
    if( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
        | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
    ) GS_ABORT();

    struct MAP_CHUNK* c= main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];

    if(c == NULL) return NULL_CONTAINER;
    return c->chunk_item_container.get(x,y,z); 
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
    GS_ASSERT(((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) == 0)
    if (((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0) return;

    struct MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    GS_ASSERT(c != NULL);
    if (c == NULL) return;
    
    c->chunk_item_container.add(x,y,z, container_type, container_id);
}

void destroy_item_container_block(int x, int y, int z)
{
    GS_ASSERT(((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) == 0)
    if (((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0) return;
    
    int val = _get(x,y,z);
    if (Item::get_container_type_for_block(val) == CONTAINER_TYPE_NONE) return;

    struct MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    GS_ASSERT(c != NULL);
    if (c == NULL) return;

    c->chunk_item_container.remove(x,y,z);
}

void get_container_location(int container_id, int position[3])
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    GS_ASSERT(container->chunk >= 0);
    if (container->chunk < 0) return;
    
    struct MAP_CHUNK* c = main_map->chunk[container->chunk];
    GS_ASSERT(c != NULL);
    if (c == NULL) return;

    c->chunk_item_container.get_container_location(container_id, position);
}

#endif



}
