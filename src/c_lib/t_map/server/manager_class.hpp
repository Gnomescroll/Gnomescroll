/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/t_map_class.hpp>
#include <t_map/t_map.hpp>

#include <t_map/net/t_StoC.hpp>

#include <common/qsort.h>

#include <physics/quadrant.hpp>

namespace t_map
{

char* compression_buffer = NULL;

const int COMPRESSION_BUFFER_SIZE = 1024*512;
extern char* compression_buffer;

const int DEFAULT_SUB_RADIUS = 128 + 32;
const int DEFAULT_UNSUB_RADIUS = 128 + 64; //128 +32; //why so low?
const int DEFAULT_UNSUB_RADIUS2 = DEFAULT_UNSUB_RADIUS*DEFAULT_UNSUB_RADIUS;


const int MAP_MANAGER_ALIAS_LIST_SIZE = 1024;

const int MAP_CHUNK_QUE_SIZE = 1024; //que for loading map chunks

//alias constants
const unsigned short NO_ALIAS = 0xffff;

const unsigned short SUBSCRIBED = 0;
const unsigned QUED = 0xffff-1;

/*
    state:
    non-subscribed, no data
    non-subscribed, version number
    subscribed

    0xffff
    0x0000
*/

class MapManagerElement
{
    public:
    unsigned short version;

    MapManagerElement()
    {
        version = NO_ALIAS;
    }
};

struct QueueElement
{
    int distance2;
    int xpos;
    int ypos;
    int index;
    short version;
};

class MapManager
{
    public:
        class TerrainMap* t;

        ClientID client_id;

        bool needs_update;
        int xpos; //player chunk position
        int ypos;

        int SUB_RADIUS;
        int SUB_RADIUS2;

        int UNSUB_RADIUS2;

        int xchunk_dim;
        int ychunk_dim;

        class MapManagerElement* version_list;

        int subed_chunks;
        int alias_list[ MAP_MANAGER_ALIAS_LIST_SIZE ];  //aliases are ints

        struct QueueElement chunk_que[MAP_CHUNK_QUE_SIZE];

        int chunk_que_num;

        //compressor

        mz_stream stream;

    MapManager(ClientID client_id)
    {
        init_compressor();

        needs_update = false;
        this->client_id = client_id;

        chunk_que_num = 0;

        SUB_RADIUS = DEFAULT_SUB_RADIUS;
        SUB_RADIUS2 = DEFAULT_SUB_RADIUS*DEFAULT_SUB_RADIUS;;

        UNSUB_RADIUS2 = DEFAULT_UNSUB_RADIUS*DEFAULT_UNSUB_RADIUS;

        subed_chunks = 0;
        xpos = 0xffff;
        ypos = 0xffff;

        t = get_map();
        xchunk_dim = t->xchunk_dim;
        ychunk_dim = t->ychunk_dim;

        version_list = new MapManagerElement[xchunk_dim*ychunk_dim];
        for (int i=0; i < MAP_MANAGER_ALIAS_LIST_SIZE; i++) alias_list[i] = NO_ALIAS;
    }

    ~MapManager()
    {
        for (int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
        {
            if (alias_list[i] == NO_ALIAS) continue;
            unsub(i);
        }

        delete[] version_list;

        end_compressor();
    }

    void update();
    void set_position(int x, int y);

    void sort_que();
    void dispatch_que();

    private:

    void que_for_sub(int x, int y);

    void sub(int index, int version);
    void unsub(int alias);


/*
    Factor out the map chunk compressor
*/
    void send_compressed_chunk(int alias, int index);
    void send_uncompressed_chunk(int alias, int index);

    void init_compressor();
    void end_compressor();

    void send_chunk_item_containers(int chunk_index);
    void send_chunk_item_container_reset(int chunk_index);

};


void MapManager::init_compressor()
{
    //int level = MZ_BEST_COMPRESSION;
    int level = 5;
    memset(&stream, 0, sizeof(stream));

    if (mz_deflateInit(&stream, level) != MZ_OK)
    {
        printf("deflateInit() failed!\n");
        return;
    }
}

void MapManager::end_compressor()
{
    mz_inflateEnd(&stream);
}


void MapManager::send_compressed_chunk(int alias, int index)
{
    if (t->chunk[index] == NULL)
    {
        printf("MapManager::send_compressed_chunk, fatal error! NULL map chunk \n");
        return;
    }
    stream.next_in = (unsigned char*) t->chunk[index]->e;
    stream.avail_in = sizeof(struct MapElement)*16*16*TERRAIN_MAP_HEIGHT;
    stream.next_out = (unsigned char*) compression_buffer;
    stream.avail_out = COMPRESSION_BUFFER_SIZE;

    int status = mz_deflate(&stream, MZ_SYNC_FLUSH); //Z_FINISH
    if (status != MZ_OK)
    {
        printf("MapManager::send_compressed_chunk: deflate() failed with status %i! \n", status);
        return;
    }

    int size = COMPRESSION_BUFFER_SIZE - stream.avail_out;  //number of bytes written
    //printf("compressed size = %i \n", size);
    map_chunk_compressed_StoC c;
    c.chunk_alias = alias;
    c.chunk_index = index;
    c.sendToClient(client_id, compression_buffer, size);
}

void MapManager::send_uncompressed_chunk(int alias, int index)
{
    if (t->chunk[index] == NULL) return;

    map_chunk_uncompressed_StoC c;

    c.chunk_alias = alias;
    c.chunk_index = index;

    int size = sizeof(struct MapElement)*16*16*TERRAIN_MAP_HEIGHT;
    //c.byte_size = size;
    c.sendToClient(client_id, (char*)t->chunk[index]->e, size);

    //void sendToClient(ClientID client_id, char* buff, int len)
    //map_chunk_uncompressed_StoC
}

static inline int MY_MIN(int x, int y)
{
    return x <= y ? x : y;
}

static inline int MY_MAX(int x, int y)
{
    return x >= y ? x : y;
}

void MapManager::update()
{
    GS_ASSERT(xpos != 0xffff && ypos != 0xffff);
    //printf("xpos= %i ypos= %i \n", xpos, ypos);
    if (!needs_update) return;

    /*
        unsub part
    */

    //printf("map manager: xpos= %i ypos= %i \n", xpos, ypos);

    int _xpos = xpos / 16;
    int _ypos = ypos / 16;

    int _SUB_RADIUS = (SUB_RADIUS/16) + 1;

    GS_ASSERT(xpos >= 0 && xpos <= 512.0);
    GS_ASSERT(ypos >= 0 && ypos <= 512.0);

    for (int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
    {
        if (alias_list[i] == NO_ALIAS) continue;  //QUED || NO_ALIAS

        int x = 16*(alias_list[i] % xchunk_dim)+8;
        int y = 16*(alias_list[i] / ychunk_dim)+8;

        //x = x - xpos;
        //y = y - ypos;

        int dx = xpos - quadrant_translate_i(xpos, x);
        int dy = ypos - quadrant_translate_i(ypos, y);

        if (dx*dx+dy*dy > UNSUB_RADIUS2) unsub(i);

    }

    /*
        sub part
    */


    const int CHUNK_BIT_MASK = 32-1;

    int imin = (_xpos - _SUB_RADIUS) & CHUNK_BIT_MASK;
    int imax = (_xpos + _SUB_RADIUS) & CHUNK_BIT_MASK;

    int jmin = (_ypos - _SUB_RADIUS) & CHUNK_BIT_MASK;
    int jmax = (_ypos + _SUB_RADIUS) & CHUNK_BIT_MASK;

    //printf("xpos= %i ypos= %i \t imin= %i imax= %i jmin= %i jmax= %i \n", xpos, ypos, imin,imax, jmin,jmax);

    //printf("imin, imax = %i %i jmin jmax = %i %i \n", imin,imax, jmin,jmax);
    for (int i=imin; i != imax; i= (i+1)& CHUNK_BIT_MASK)
    for (int j=jmin; j != jmax; j= (j+1)& CHUNK_BIT_MASK)
    {
        //i &= TERRAIN_MAP_WIDTH_BIT_MASK2;


        unsigned int version = version_list[j*xchunk_dim + i].version;
        if (version == SUBSCRIBED || version == QUED) continue;

/*
    Quadrant Math
*/
        int x = xpos - quadrant_translate_i(xpos, i*16 + 8);
        int y = ypos - quadrant_translate_i(ypos, j*16 + 8);

        if (x*x+y*y >= SUB_RADIUS2) continue;
        //int x = xpos - (i*16 + 8);
        //int y = ypos - (j*16 + 8);

        //if (x + y*y >= SUB_RADIUS2) continue;
        //printf("sub %i %i \n", i,j);

        if (i<0 || i >= xchunk_dim || j<0 || j >= ychunk_dim) printf("MapManager::update(), ERROR!!!\n");

        que_for_sub(i,j);
        //printf("que_for_sub: i,j= %i %i \t xpos= %i ypos= %i \n", i,j, xpos, ypos);

    }

    this->sort_que();
    needs_update = false;
}

//put chunk onto the que to be subscribed
void MapManager::que_for_sub(int x, int y)
{

    int index = y*xchunk_dim + x;

    if (version_list[index].version == QUED)
    {
        printf("Error: MapManager::que_for_sub, adding to que twice!\n");
        return;
    }

    if (chunk_que_num == MAP_CHUNK_QUE_SIZE)
    {
        printf("MapManager::que_for_sub: Warning chunk_que_num == MAP_CHUNK_QUE_SIZEs \n");
        return;
    }


    struct QueueElement q;

    q.version = version_list[index].version;   //save version

    q.xpos = 16*x + 8;
    q.ypos = 16*y + 8;
    q.index = index;

    chunk_que[chunk_que_num] = q;
    chunk_que_num++;
    //set version, so it is not requed
    version_list[index].version = QUED; //set subscription property, so it does not get requed
}

/*
static inline void QUE_ELEMENT_qsort(struct QueueElement *arr, unsigned int n, int xpos, int ypos)
{
    for (size_t i=0; i<n; i++)
    {
        int x = xpos - quadrant_translate_i(xpos, arr[i].xpos);
        int y = ypos - quadrant_translate_i(ypos, arr[i].ypos);
        arr[i].distance2 = x*x + y*y;
    }

    #define QUE_ELEMENT_lt(a,b) ((a)->distance2 > (b)->distance2)
    QSORT(struct QueueElement, arr, n, QUE_ELEMENT_lt);
}

void MapManager::sort_que()
{
    if (chunk_que_num == 0) return;
    QUE_ELEMENT_qsort(chunk_que, chunk_que_num, xpos, ypos);
}
*/

void MapManager::sort_que()
{
    if (chunk_que_num == 0) return;
    QueueElement *arr = chunk_que;
    const int n = chunk_que_num;

    for (int i=0; i<chunk_que_num; i++)
    {
        int x = xpos - quadrant_translate_i(xpos, arr[i].xpos);
        int y = ypos - quadrant_translate_i(ypos, arr[i].ypos);
        arr[i].distance2 = x*x + y*y;
    }

    #define QUE_ELEMENT_lt(a,b) ((a)->distance2 > (b)->distance2)
    QSORT(struct QueueElement, arr, n, QUE_ELEMENT_lt);
}

void MapManager::dispatch_que()
{
/*
    Future: Check if player is in radius and if not, then skip
*/
    if (chunk_que_num == 0) return;


    while (chunk_que_num > 0)
    {
        struct QueueElement* q = &chunk_que[chunk_que_num-1];

        int dx = xpos - quadrant_translate_i(xpos, q->xpos);
        int dy = ypos - quadrant_translate_i(ypos, q->ypos);
        int distance2 = dx*dx + dy*dy;
        if (distance2 <= SUB_RADIUS2)
        {
            sub(q->index, q->version);
            chunk_que_num--;
            break;
        }
        else
        {
            //skip because its out of range
            version_list[q->index].version = q->index;
            chunk_que_num--;
        }
    }


}


void MapManager::send_chunk_item_containers(int chunk_index)
{
    if (t->chunk[chunk_index] == NULL) return;
    t->chunk[chunk_index]->chunk_item_container.send_chunk_item_containers(client_id);
}

void MapManager::send_chunk_item_container_reset(int chunk_index)
{
    if (t->chunk[chunk_index] == NULL) return;
    t->chunk[chunk_index]->chunk_item_container.send_reset_chunk_item_containers(client_id);
}

void MapManager::set_position(int x, int y)
{
    if (x != xpos || y != ypos) needs_update = true;
    xpos = x;
    ypos = y;
}

void MapManager::sub(int chunk_index, int version)
{
    //printf("sub: %i %i \n", x, y);
    //int index = y*xchunk_dim + x;

    //printf("sub chunk %i \n", chunk_index);

    if (subed_chunks == MAP_MANAGER_ALIAS_LIST_SIZE)
    {
        printf("FIX THIS!!! MapManager::sub, alias list maxed out \n");
        return;
    }
    int current_version = version_list[chunk_index].version;
    version_list[chunk_index].version = SUBSCRIBED;
    subed_chunks++;

    int alias=0;
    while (alias_list[alias] != NO_ALIAS) alias++;
    alias_list[alias] = chunk_index;

    //send subscription message

    map_history->chunk[chunk_index].add_subscriber(client_id, alias, current_version);
    send_compressed_chunk(alias, chunk_index);
    send_chunk_item_containers(chunk_index);

}

void MapManager::unsub(int alias)
{
    int chunk_index = alias_list[alias];


    if (version_list[chunk_index].version == QUED)
    {
        printf("Error: MapManager::unsub, unsubbing element on que!? wtf\n");
        return;
    }
    //printf("unsub: %i %i \n", index % xchunk_dim, index / xchunk_dim);
    version_list[chunk_index].version = map_history->chunk[chunk_index].version; //cache version

    map_history->chunk[chunk_index].remove_subscriber(client_id);
    send_chunk_item_container_reset(chunk_index);

    alias_list[alias] = NO_ALIAS;
    subed_chunks--;

    //notify client that they are no longer receiving chunk updates
    class clear_alias_StoC p;
    p.chunk_alias = alias;
    p.sendToClient(client_id);


    //send unsubscribe message
}


}   // t_map

