#include "_interface.hpp"

#if DC_CLIENT
#include <t_map/t_vbo_class.hpp>
#include <t_map/t_vbo_update.hpp>
#include <t_map/t_vbo_draw.hpp>
#endif

#include <t_map/common/constants.hpp>

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>

namespace t_map
{

void init_packets()
{
    t_map::map_chunk_compressed_StoC::register_client_packet();
    t_map::map_chunk_uncompressed_StoC::register_client_packet();

    t_map::set_map_alias_StoC::register_client_packet();
    t_map::clear_alias_StoC::register_client_packet();

    // block value change
    t_map::block_set_StoC::register_client_packet();
    t_map::block_set_palette_StoC::register_client_packet();

    t_map::block_action_StoC::register_client_packet();
    t_map::map_metadata_StoC::register_client_packet();
    
    // block damage
    t_map::request_block_damage_CtoS::register_server_packet();
    t_map::block_damage_StoC::register_client_packet();
    
    t_map::container_block_chunk_reset_StoC::register_client_packet();
    t_map::container_block_create_StoC::register_client_packet();
    t_map::container_block_delete_StoC::register_client_packet();

    t_map::control_node_create_StoC::register_client_packet();
    t_map::control_node_delete_StoC::register_client_packet();
}


int get_block_item_container(int x, int y, int z)
{
    if ( ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK)
        | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0 
    ) GS_ABORT();

    class MAP_CHUNK* c= main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];

    if (c == NULL) return NULL_CONTAINER;
    return c->chunk_item_container.get(x,y,z); 
}

bool get_container_location(int container_id, int position[3])
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;
    GS_ASSERT(container->chunk >= 0);
    if (container->chunk < 0) return false;
    GS_ASSERT(container->chunk < main_map->xchunk_dim*main_map->ychunk_dim);
    if (container->chunk >= main_map->xchunk_dim*main_map->ychunk_dim) return false;
    
    class MAP_CHUNK* c = main_map->chunk[container->chunk];
    GS_ASSERT(c != NULL);
    if (c == NULL) return false;

    c->chunk_item_container.get_container_location(container_id, position);
    return true;
}


#if DC_CLIENT
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);

        t_vbo_update_init();

        vbo_draw_init();

    }

    void teardown_t_vbo()
    {
        delete vbo_map;
        t_vbo_update_end();
        vbo_draw_end();
    }

    void draw_map()
    {
        vbo_map->draw_map();
    }
    
    void draw_map_compatibility()
    {
        vbo_map->draw_map_compatibility();
    }

    void update_map()
    {
        vbo_map->update_map();
    }
    
    
static const unsigned int REQUEST_DMG_ID_MAX = 0xff;
static unsigned int request_id = 0;

static struct { int x,y,z; } last_requested_block = {-1};
static unsigned int last_request_id = 0;
static int requested_block_health = 0;

CubeID requested_cube_id = NULL_CUBE;
unsigned int requested_block_damage = 0;

bool is_last_requested_block(int x, int y, int z)
{
    return (
        last_requested_block.x == x
     && last_requested_block.y == y
     && last_requested_block.z == z);
}

void request_block_damage(int x, int y, int z)
{
    GS_ASSERT(request_id < REQUEST_DMG_ID_MAX);
    
    // ignore if we already requested this block
    if (is_last_requested_block(x,y,z)) return;
    
    // set last request id & block
    last_request_id = request_id;
    last_requested_block.x = x;
    last_requested_block.y = y;
    last_requested_block.z = z;
    
    requested_cube_id = t_map::get(x,y,z);
    requested_block_health = maxDamage(requested_cube_id);
    
    // send packet
    request_block_damage_CtoS msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.request_id = request_id;
    msg.send();
    
    // increment index
    request_id++;
    request_id %= REQUEST_DMG_ID_MAX;
    
    // reset dmg request
    requested_block_damage = 0;
}

int get_requested_block_remaining_health()
{
    int health = requested_block_health - requested_block_damage;
    GS_ASSERT(health >= 0);
    if (health < 0) health = 0;
    return health;
}

void get_requested_block_position(int* x, int* y, int* z)
{
    *x = last_requested_block.x;
    *y = last_requested_block.y;
    *z = last_requested_block.z;
}

void received_block_damage_response(unsigned int request_id, unsigned int dmg)
{
    GS_ASSERT(request_id < REQUEST_DMG_ID_MAX);
    if (request_id >= REQUEST_DMG_ID_MAX) return;
    // if request_id matches last request id,
    if (request_id == last_request_id)
        requested_block_damage = dmg;   // set current dmg
}
#endif

#if DC_SERVER
void create_item_container_block(int x, int y, int z, ItemContainerType container_type, int container_id)
{
    GS_ASSERT(((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) == 0)
    if (((z & TERRAIN_MAP_HEIGHT_BIT_MASK) | (x & TERRAIN_MAP_WIDTH_BIT_MASK) | (y & TERRAIN_MAP_WIDTH_BIT_MASK)) != 0) return;

    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    GS_ASSERT(c != NULL);
    if (c == NULL) return;
    
    c->chunk_item_container.add(x,y,z, container_type, container_id);
}

void destroy_item_container_block(int x, int y, int z)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0)
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    
    CubeID val = get(x,y,z);
    if (Item::get_container_type_for_block(val) == CONTAINER_TYPE_NONE) return;

    class MAP_CHUNK* c = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    GS_ASSERT(c != NULL);
    if (c == NULL) return;

    c->chunk_item_container.remove(x,y,z);
}

void smelter_on(int container_id)
{
    int p[3] = {0};
    bool success = get_container_location(container_id, p);
    if (!success) return;
    int x = p[0];
    int y = p[1];
    int z = p[2];
    int palette = get_palette(x,y,z);
    if (palette >= 4) return;
    palette += 4;
    broadcast_set_palette(x,y,z, palette);
}

void smelter_off(int container_id)
{
    int p[3] = {0};
    bool success = get_container_location(container_id, p);
    if (!success) return;
    int x = p[0];
    int y = p[1];
    int z = p[2];
    int palette = get_palette(x,y,z);
    if (palette < 4) return;
    palette -= 4;
    broadcast_set_palette(x,y,z, palette);
}

/*
    Do on client connect
*/
void send_client_map_special(ClientID client_id)
{
    main_map->control_node_list.send_control_nodes_to_client(client_id);
}

void add_control_node(int x, int y, int z)
{
    //printf("Server adding control node at: %i %i %i \n", x,y,z);
    main_map->control_node_list.server_add_control_node(x,y,z); 
}

#endif

}   // t_map

