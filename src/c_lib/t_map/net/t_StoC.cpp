#include "t_StoC.hpp"

#include <t_map/net/t_CtoS.hpp>
#include <t_map/t_map.hpp>

#if DC_CLIENT

#include <animations/animations.hpp>
#include <common/random.hpp>
#include <sound/sound.hpp>

#endif

namespace t_map
{

#if DC_CLIENT


int client_chunk_alias_list[1024] = {0};

mz_stream stream;

const int DECOMPRESSION_BUFFER_SIZE = 1024*512;
char* decompression_buffer = NULL;

#define MAP_NET_DEBUG 0
//unsigned short chunk_alias;
//int chunk_index;

void init_client_compressors()
{
    // Init the z_stream
    decompression_buffer = (char*) malloc(DECOMPRESSION_BUFFER_SIZE);

    memset(&stream, 0, sizeof(stream));

    if (mz_inflateInit(&stream))
    {
        printf("map_chunk_compressed_StoC::handle, inflateInit() failed!\n");
    }

    for (int i=0; i<1024; i++)
        client_chunk_alias_list[i] = -1;
    return;
}

void end_client_compressors()
{
    free(decompression_buffer);
}

void map_chunk_compressed_StoC::handle(char* buff, size_t byte_num)
{
    GS_ASSERT(client_chunk_alias_list[chunk_alias] == -1);

    stream.next_in = (unsigned char*) buff;
    stream.avail_in = byte_num;

    stream.next_out = (unsigned char*) decompression_buffer;
    stream.avail_out = DECOMPRESSION_BUFFER_SIZE;

    int status = mz_inflate(&stream, MZ_SYNC_FLUSH);

    if (status != MZ_OK)
    {
        printf("inflate() failed with status %d!\n", status);
        return;
    }


    int size = DECOMPRESSION_BUFFER_SIZE - stream.avail_out;

    //printf("compressed chunk: %d bytes decompressed to %d bytes \n", byte_num, size);

    /*
        Handle
    */

    client_chunk_alias_list[chunk_alias] = chunk_index;

    //printf("received chunk: index = %d compressed \n", chunk_index);


    GS_ASSERT(main_map->chunk[chunk_index] == NULL);

    GS_ASSERT(MAP_CHUNK_XDIM == 32 && MAP_CHUNK_YDIM == 32);
    int cx = chunk_index % MAP_CHUNK_XDIM;
    int cy = chunk_index / MAP_CHUNK_XDIM;

    main_map->load_chunk(cx, cy);
    class MapChunk* m = main_map->chunk[chunk_index];

    int _size = sizeof(struct MapElement)*16*16*TERRAIN_MAP_HEIGHT;

    if (size != _size) printf("map_chunk_compressed_StoC::handle, warning: invalid size!\n");

    memcpy((char *) m->e, decompression_buffer, _size);

    m->refresh_height_cache(); //refesh height cache after memcpy
    main_map->chunk_received(cx,cy);
}

void map_chunk_uncompressed_StoC::handle(char* buff, size_t byte_num)
{
    //printf("map_chunk: alias= %d for %d %d \n", chunk_alias, chunk_index%MAP_CHUNK_XDIM, chunk_index /MAP_CHUNK_XDIM);
    //printf("byte_size= %d \n", byte_size);
#if MAP_NET_DEBUG
    printf("map chunk is %d bytes \n", byte_size);
#endif
    GS_ASSERT(client_chunk_alias_list[chunk_alias] == -1);
    client_chunk_alias_list[chunk_alias] = chunk_index;

    int cx = chunk_index % MAP_CHUNK_XDIM;
    int cy = chunk_index / MAP_CHUNK_XDIM;

    GS_ASSERT(main_map->chunk[chunk_index] == NULL);
    main_map->load_chunk(cx, cy);
    class MapChunk* m = main_map->chunk[chunk_index];

/*
    This is evil, dont do this
*/

    memcpy((char *) m->e, buff, byte_num);
    m->refresh_height_cache(); //refesh height cache after memcpy

    main_map->chunk_received(cx,cy);
}


//unsigned short chunk_alias;//
void clear_alias_StoC::handle()
{
    int chunk_index = client_chunk_alias_list[chunk_alias];

#if 0
    int _x = client_chunk_alias_list[chunk_alias]%32;
    int _y = client_chunk_alias_list[chunk_alias]/32;
    printf("cleared chunk alias: %d %d \n", _x,_y);
#endif

    GS_ASSERT(main_map->chunk[chunk_index] != NULL);

    int cx = chunk_index % MAP_CHUNK_XDIM;
    int cy = chunk_index / MAP_CHUNK_XDIM;

    main_map->unload_chunk(cx, cy);

    /*
        Dump map chunk
    */
    client_chunk_alias_list[chunk_alias] = -1;
}

//unsigned short chunk_alias;
//unsigned int chunk_index;
void set_map_alias_StoC::handle()
{
    client_chunk_alias_list[chunk_alias] = chunk_index;
    printf("Alias %d set to %d %d \n", chunk_alias, chunk_index%MAP_CHUNK_XDIM, chunk_index /MAP_CHUNK_XDIM);
}

void map_element_update::handle()
{
    printf("received map element update \n");
}


void block_set_StoC::handle()
{
    IF_ASSERT(!is_boxed_position(this->position)) return;
    set(this->position, (CubeType)cube_type);
}

void block_set_palette_StoC::handle()
{
    struct MapElement e = NULL_MAP_ELEMENT;
    //struct MapElement e = get_element(x,y,z);
    //GS_ASSERT(e.block == cube_type);        //this assert might be wrong
    e.block = cube_type;
    e.palette = palette;
    //e.light   = fast_cube_attributes[cube_type].light_value;

    set_element(this->position, e);
}

void block_action_StoC::handle()
{
    struct Vec3 p = vec3_scalar_add(vec3_init(this->position), 0.5f);
    if ((CubeType)this->cube_type == EMPTY_CUBE)
    {
        CubeType old_cube_type = get(this->position);
        Animations::block_crumble(p, randrange(10, 30), old_cube_type,
                                  (TerrainModificationAction)action);
        Sound::play_3d_sound("block_destroyed", p);
    }
    else
    {
        Sound::play_3d_sound("block_set", p);
    }
    set(this->position, (CubeType)this->cube_type);
}

void map_metadata_StoC::handle()
{
    map_dim = this->dimensions;
}

/*
    Container cube_type
*/

void container_block_chunk_reset_StoC::handle()
{
    if (chunk_index >= (unsigned int)(main_map->xchunk_dim*main_map->ychunk_dim)) return;
    IF_ASSERT(main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container._reset();
};


void container_block_create_StoC::handle()
{
    if (!is_valid_z(this->position)) return;
    Vec3i p = translate_position(this->position);
    int chunk_index = (p.y / 16) * (map_dim.x / 16) + (p.x / 16);
    IF_ASSERT(main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container.add(
        p, (ItemContainerType)container_type, (ItemContainerID)container_id);
}

void container_block_delete_StoC::handle()
{
    if (chunk_index >= (unsigned int)(main_map->xchunk_dim*main_map->ychunk_dim)) return;
    IF_ASSERT(main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container.remove((ItemContainerID)container_id);
}


/*
    Control Points
*/

//    uint16_t x,y,z;
void control_node_create_StoC::handle()
{
    main_map->control_node_list.add_control_node(this->position);
    main_map->control_node_list.needs_update = true;
};

//    uint16_t x,y,z;
void control_node_delete_StoC::handle()
{
    main_map->control_node_list.remove_control_node(this->position);
    main_map->control_node_list.needs_update = true;
};

/* block damage */

void block_damage_StoC::handle()
{
    received_block_damage_response(request_id, dmg);
}

#endif


#if DC_SERVER

void map_chunk_compressed_StoC::handle(char* buff, size_t byte_num) {}
void map_chunk_uncompressed_StoC::handle(char* buff, size_t byte_size) {}

void clear_alias_StoC::handle() {}
void set_map_alias_StoC::handle() {}

void block_set_StoC::handle() {}
void block_set_palette_StoC::handle() {}

void block_action_StoC::handle() {}
void map_metadata_StoC::handle() {}

void container_block_chunk_reset_StoC::handle() {}
void container_block_create_StoC::handle() {}
void container_block_delete_StoC::handle() {}

void control_node_create_StoC::handle() {}
void control_node_delete_StoC::handle() {}

void block_damage_StoC::handle() {}
#endif

}   // t_map
