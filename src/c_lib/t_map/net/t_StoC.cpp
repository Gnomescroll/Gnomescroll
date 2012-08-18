#include "t_StoC.hpp"

#include "t_CtoS.hpp"

#include "../t_map.hpp"
//#include "../t_map_class.hpp"

namespace t_map
{

#if DC_CLIENT

#include <animations/animations.hpp>
#include <common/random.h>
#include <sound/triggers.hpp>


int client_chunk_alias_list[1024] = {0};

mz_stream stream;

const int DECOMPRESSION_BUFFER_SIZE = 1024*512;
char* DECOMPRESSION_BUFFER = NULL;

#define MAP_NET_DEBUG 0
//unsigned short chunk_alias;
//int chunk_index;

void init_client_compressors()
{
    // Init the z_stream
    DECOMPRESSION_BUFFER = (char*) malloc(DECOMPRESSION_BUFFER_SIZE);

    memset(&stream, 0, sizeof(stream));

    if (mz_inflateInit(&stream))
    {
        printf("map_chunk_compressed_StoC::handle, inflateInit() failed!\n");
    }

    return;
}

void end_client_compressors()
{
    free(DECOMPRESSION_BUFFER);

}

void map_chunk_compressed_StoC::handle(char* buff, int byte_num)
{


    stream.next_in = (unsigned char*) buff;
    stream.avail_in = byte_num;

    stream.next_out = (unsigned char*) DECOMPRESSION_BUFFER;
    stream.avail_out = DECOMPRESSION_BUFFER_SIZE;

    int status = mz_inflate(&stream, MZ_SYNC_FLUSH);

    if (status != MZ_OK)
    {
        printf("inflate() failed with status %i!\n", status);
        return;
    }


    int size = DECOMPRESSION_BUFFER_SIZE - stream.avail_out;

    //printf("compressed chunk: %i bytes decompressed to %i bytes \n", byte_num, size);
    
    /*
        Handle
    */

    client_chunk_alias_list[chunk_alias] = chunk_index;

    //printf("received chunk: index = %i compressed \n", chunk_index);
    
    
    GS_ASSERT( main_map->chunk[chunk_index] == NULL);

    int cx = chunk_index % MAP_CHUNK_XDIM;
    int cy = chunk_index / MAP_CHUNK_XDIM;

    if(m == NULL)
        main_map->load_chunk(cx, cy);
    
    class MAP_CHUNK* m = main_map->chunk[chunk_index];

    int _size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;

    if(size != _size) printf("map_chunk_compressed_StoC::handle, warning: invalid size!\n");

    memcpy( (char *) m->e, DECOMPRESSION_BUFFER, _size);
    main_map->chunk_received(x,y);

}

void map_chunk_uncompressed_StoC::handle(char* buff, int byte_num)
{
    //printf("map_chunk: alias= %i for %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_XDIM, chunk_index /MAP_CHUNK_XDIM );
    //printf("byte_size= %i \n", byte_size);
#if MAP_NET_DEBUG
    printf("map chunk is %i bytes \n", byte_size);
#endif

    client_chunk_alias_list[chunk_alias] = chunk_index;



    int cx = chunk_index % MAP_CHUNK_XDIM;
    int cy = chunk_index / MAP_CHUNK_XDIM;

    GS_ASSERT( main_map->chunk[chunk_index] == NULL);
    if(m == NULL)
        main_map->load_chunk(cx, cy);
    class MAP_CHUNK* m = main_map->chunk[chunk_index];

/*
    This is evil, dont do this
*/
    class MAP_CHUNK* m = main_map->chunk[chunk_index];

    memcpy( (char *) m->e, buff, byte_num);

}


//unsigned short chunk_alias;//
void clear_alias_StoC::handle()
{
    int chunk_index = client_chunk_alias_list[chunk_alias];
    
#if 0
    int _x = client_chunk_alias_list[chunk_alias]%32;
    int _y = client_chunk_alias_list[chunk_alias]/32;
    printf("cleared chunk alias: %i %i \n", _x,_y);
#endif

    GS_ASSERT( main_map->chunk[chunk_index] != NULL);

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
    //printf("Alias %i set to %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_XDIM, chunk_index /MAP_CHUNK_XDIM);
}

void map_element_update::handle()
{
    printf("received map element update \n");
}


void block_set_StoC::handle() 
{
    GS_ASSERT(x < map_dim.x && y < map_dim.y && z < map_dim.z);
    if (x >= map_dim.x || y >= map_dim.y || z >= map_dim.z) return;
    main_map->set_block(x,y,z, block);
}

void block_set_palette_StoC::handle()
{
    struct MAP_ELEMENT e = {{{0}}};
    e.block = block;
    e.palette = palette;

    main_map->set_element(x,y,z, e);
}

void block_action_StoC::handle()
{
    if (block == 0) 
    {
        int cube_id = get(x,y,z);
        Animations::block_crumble((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, randrange(10,30), cube_id, (TerrainModificationAction)action);
        Sound::block_destroyed(x+0.5f,y+0.5f,z+0.5f, 0,0,0);
    }
    else
        Sound::block_set(x+0.5f,y+0.5f,z+0.5f,0,0,0);

    main_map->set_block(x,y,z, block);
}

void map_metadata_StoC::handle() 
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
}

/*
    Container block
*/

void container_block_chunk_reset_StoC::handle()
{
    if (chunk_index >= (unsigned int)(main_map->xchunk_dim*main_map->ychunk_dim)) return;
    GS_ASSERT(main_map->chunk[chunk_index] != NULL);
    if (main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container._reset();
};


void container_block_create_StoC::handle()
{
    if((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    int chunk_index = (y/16)*(MAP_WIDTH/16) + (x/16);
    GS_ASSERT(main_map->chunk[chunk_index] != NULL);
    if (main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container.add(x,y,z, (ItemContainerType)container_type, container_id);
}

void container_block_delete_StoC::handle()
{
    if (chunk_index >= (unsigned int)(main_map->xchunk_dim*main_map->ychunk_dim)) return;
    GS_ASSERT(main_map->chunk[chunk_index] != NULL);
    if (main_map->chunk[chunk_index] == NULL) return;
    main_map->chunk[chunk_index]->chunk_item_container.remove(container_id);
}


/*
    Control Points
*/

//    uint16_t x,y,z;
void control_node_create_StoC::handle()
{
    printf("client adding control node at: %i %i %i \n", x,y,z);
    main_map->control_node_list.add_control_node(x,y,z);
    main_map->control_node_list.needs_update = true;
};

//    uint16_t x,y,z;
void control_node_delete_StoC::handle()
{
    printf("client removing control node at: %i %i %i \n", x,y,z);
    main_map->control_node_list.remove_control_node(x,y,z);
    main_map->control_node_list.needs_update = true;
};

/* block damage */

void block_damage_StoC::handle()
{
    received_block_damage_response(request_id, dmg);
}

#endif


#if DC_SERVER

void map_chunk_compressed_StoC::handle(char* buff, int byte_num) {}
void map_chunk_uncompressed_StoC::handle(char* buff, int byte_size) {}

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
