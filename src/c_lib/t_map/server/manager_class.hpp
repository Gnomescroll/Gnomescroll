#pragma once

#include <t_map/t_map_class.hpp>
#include <t_map/t_map.hpp>

#include <t_map/net/t_StoC.hpp>

#include <common/qsort.h>

#include <physics/quadrant.hpp>

namespace t_map
{

char* COMPRESSION_BUFFER = NULL;

const int COMPRESSION_BUFFER_SIZE = 1024*512;
extern char* COMPRESSION_BUFFER;

const int DEFAULT_SUB_RADIUS = 128 + 8;
const int DEFAULT_UNSUB_RADIUS = 128 +32;
const int DEFAULT_UNSUB_RADIUS2 = DEFAULT_UNSUB_RADIUS*DEFAULT_UNSUB_RADIUS;

const int MAP_MANAGER_ALIAS_LIST_SIZE = 512;

const int MAP_CHUNK_QUE_SIZE = 512; //que for loading map chunks

//alias constants
const unsigned short NO_ALIAS = 0xffff;

const unsigned short SUBSCRIBED = 0;
const unsigned UNSUBSCRIBED_NO_DATA = 0xffff;
const unsigned QUED = 0xffff-1;

/*
    state:
    non-subscribed, no data
    non-subscribed, version number
    subscribed

    0xffff
    0x0000
*/

class MAP_MANAGER_ELEMENT
{
    public:
    unsigned short version;

    MAP_MANAGER_ELEMENT()
    {
        version = UNSUBSCRIBED_NO_DATA;    
    }
};

struct QUE_ELEMENT
{
    int distance2;
    int xpos;
    int ypos;
    int index;
    short version;
};

class Map_manager
{
    public:
    class Terrain_map* t;

    int client_id;

    bool needs_update;
    int xpos; //player chunk position
    int ypos;

    int SUB_RADIUS;
    int UNSUB_RADIUS2;

    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_MANAGER_ELEMENT* version_list;
    
    int subed_chunks;
    int alias_list[ MAP_MANAGER_ALIAS_LIST_SIZE ];  //aliases are ints

    struct QUE_ELEMENT chunk_que[MAP_CHUNK_QUE_SIZE];

    int chunk_que_num;

    //compressor

    mz_stream stream;

    Map_manager(int _client_id)
    {
        init_compressor();

        needs_update = false;
        client_id = _client_id;
        
        chunk_que_num = 0;

        SUB_RADIUS = DEFAULT_SUB_RADIUS;
        UNSUB_RADIUS2 = DEFAULT_UNSUB_RADIUS2;

        subed_chunks = 0;
        xpos = 0xffff;
        ypos = 0xffff;

        t = get_map();
        xchunk_dim = t->xchunk_dim;
        ychunk_dim = t->ychunk_dim;

        version_list = new MAP_MANAGER_ELEMENT[xchunk_dim*ychunk_dim];
        for(int i=0; i < MAP_MANAGER_ALIAS_LIST_SIZE; i++) alias_list[i] = NO_ALIAS;
    }

    ~Map_manager()
    {
        for(int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
        {
            if( alias_list[i] == NO_ALIAS) continue;
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

    void send_subscribe_chunk(int chunk_index, int alias);  //implement
    void send_unsubscribe_chunk(int chunk_index, int alias);//implement

    void send_chunk_item_containers(int chunk_index);
    void send_chunk_item_container_reset(int chunk_index);

};


void Map_manager::init_compressor()
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

void Map_manager::end_compressor()
{
    mz_inflateEnd(&stream);
}


void Map_manager::send_compressed_chunk(int alias, int index)
{
    if(t->chunk[index] == NULL)
    {
        printf("Map_manager::send_compressed_chunk, fatal error! NULL map chunk \n");
        return;
    }
    stream.next_in = (unsigned char*) t->chunk[index]->e;
    stream.avail_in = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;
    stream.next_out = (unsigned char*) COMPRESSION_BUFFER;
    stream.avail_out = COMPRESSION_BUFFER_SIZE;  

    int status = mz_deflate(&stream, MZ_SYNC_FLUSH); //Z_FINISH
    if (status != MZ_OK)
    {
        printf("Map_manager::send_compressed_chunk: deflate() failed with status %i! \n", status);
        return;
    }

    int size = COMPRESSION_BUFFER_SIZE - stream.avail_out;  //number of bytes written
    //printf("compressed size = %i \n", size);
    map_chunk_compressed_StoC c;
    c.chunk_alias = alias;
    c.chunk_index = index;
    c.sendToClient(client_id, COMPRESSION_BUFFER, size);
}

void Map_manager::send_uncompressed_chunk(int alias, int index)
{
    if(t->chunk[index] == NULL) return;

    map_chunk_uncompressed_StoC c;

    c.chunk_alias = alias;
    c.chunk_index = index;

    int size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;
    //c.byte_size = size;
    c.sendToClient(client_id, (char*)t->chunk[index]->e, size);

    //void sendToClient(int client_id, char* buff, int len) 
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

void Map_manager::update()
{
    //printf("xpos= %i ypos= %i \n", xpos, ypos);
    if(needs_update == false) return;

    /*
        unsub part
    */

    int _xpos = xpos / 16;
    int _ypos = ypos / 16;  

    int _SUB_RADIUS = (SUB_RADIUS/16) + 1;

    GS_ASSERT(xpos >= 0 && xpos <= 512.0);
    GS_ASSERT(ypos >= 0 && ypos <= 512.0);

    for(int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
    {
        if( alias_list[i] == NO_ALIAS) continue;  //QUED || NO_ALIAS 

        int x = 16*(alias_list[i] % xchunk_dim)+8;
        int y = 16*(alias_list[i] / ychunk_dim)+8;

        //x = x - xpos;
        //y = y - ypos; 

        int dx = quadrant_distance2i(xpos, x);
        int dy = quadrant_distance2i(ypos, y);

        if( dx+dy > UNSUB_RADIUS2 ) unsub(i);

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

    int SUB_RADIUS2 = SUB_RADIUS*SUB_RADIUS;

    //printf("imin, imax = %i %i jmin jmax = %i %i \n", imin,imax, jmin,jmax);
    for(int i=imin; i != imax; i= (i+1)& CHUNK_BIT_MASK )
    for(int j=jmin; j != jmax; j= (j+1)& CHUNK_BIT_MASK )
    {
        //i &= TERRAIN_MAP_WIDTH_BIT_MASK2;


        unsigned int version = version_list[j*xchunk_dim + i].version;
        if( version == SUBSCRIBED || version == QUED ) continue;

/*
    Quadrant Math
*/  
        int x = quadrant_distance2i(xpos, i*16 + 8);
        int y = quadrant_distance2i(ypos, j*16 + 8);

        if(x+y >= SUB_RADIUS2 ) continue;
        //int x = xpos - (i*16 + 8);
        //int y = ypos - (j*16 + 8);

        //if( x + y*y >= SUB_RADIUS2 ) continue;
        //printf("sub %i %i \n", i,j);

        if(i<0 || i >= xchunk_dim || j<0 || j >= ychunk_dim) printf("Map_manager::update(), ERROR!!!\n");

        que_for_sub(i,j);
        //printf("que_for_sub: i,j= %i %i \t xpos= %i ypos= %i \n", i,j, xpos, ypos);

    }


    needs_update = false;
}

//put chunk onto the que to be subscribed
void Map_manager::que_for_sub(int x, int y)
{

    if( chunk_que_num == MAP_CHUNK_QUE_SIZE)
    {
        printf("Map_manager::que_for_sub: Warning chunk_que_num == MAP_CHUNK_QUE_SIZEs \n" );
        return;
    }

    int index = y*xchunk_dim + x;

    if(version_list[index].version == QUED)
    {
        printf("Error: Map_manager::que_for_sub, adding to que twice!\n");
        return;
    }

    struct QUE_ELEMENT q; 

    q.version = version_list[index].version;   //save version
    //printf("version = %hx \n", q.version);

    {
        //int _x = xpos - (x*16 + 8);
        //int _y = ypos - (y*16 + 8);
        //q.distance2 = _x*_x + _y*_y;
        q.xpos = 16*x + 8;
        q.ypos = 16*y + 8;
    }
    q.index = index;

    chunk_que[chunk_que_num] = q;
    chunk_que_num++;
    //set version, so it is not requed
    version_list[index].version = QUED; //set subscription property, so it does not get requed
}

static inline void QUE_ELEMENT_qsort(struct QUE_ELEMENT *arr, unsigned int n, int xpos, int ypos) 
{
    for(unsigned int i=0; i<n; i++)
    {
        int x = xpos - quadrant_distance2i(xpos, arr[i].xpos);
        int y = ypos - quadrant_distance2i(xpos, arr[i].ypos);
        arr[i].distance2 = x*x + y*y;
    }

    #define QUE_ELEMENT_lt(a,b) ((a)->distance2 > (b)->distance2)
    QSORT(struct QUE_ELEMENT, arr, n, QUE_ELEMENT_lt );
}

void Map_manager::sort_que()
{
    if(chunk_que_num == 0) return;
    QUE_ELEMENT_qsort(chunk_que, chunk_que_num, xpos, ypos);
}

void Map_manager::dispatch_que()
{
    if( chunk_que_num == 0) return;
    struct QUE_ELEMENT* q = &chunk_que[chunk_que_num-1];
    sub(q->index, q->version);
    chunk_que_num--;
}


void Map_manager::send_subscribe_chunk(int chunk_index, int alias)
{}

void Map_manager::send_unsubscribe_chunk(int chunk_index, int alias)
{}

void Map_manager::send_chunk_item_containers(int chunk_index)
{
    if(t->chunk[chunk_index] == NULL) return;
    t->chunk[chunk_index]->chunk_item_container.send_chunk_item_containers(client_id);
}

void Map_manager::send_chunk_item_container_reset(int chunk_index)
{
    if(t->chunk[chunk_index] == NULL) return;
    t->chunk[chunk_index]->chunk_item_container.send_reset_chunk_item_containers(client_id);
}

void Map_manager::set_position(int x, int y)
{
    if(x != xpos || y != ypos) needs_update = true;
    xpos = x;
    ypos = y;
}

void Map_manager::sub(int chunk_index, int version)
{
    //printf("sub: %i %i \n", x, y);
    //int index = y*xchunk_dim + x;

    //printf("sub chunk %i \n", chunk_index);

    if( subed_chunks == MAP_MANAGER_ALIAS_LIST_SIZE)
    {
        printf("FIX THIS!!! Map_manager::sub, alias list maxed out \n");
        return;
    }
    int current_version = version_list[chunk_index].version;
    version_list[chunk_index].version = SUBSCRIBED;
    subed_chunks++;

    int alias=0;
    while( alias_list[alias] != NO_ALIAS) alias++;
    alias_list[alias] = chunk_index;

    //send subscription message

    map_history->chunk[chunk_index].add_subscriber(client_id, alias, current_version);
    send_compressed_chunk(alias, chunk_index);
    send_chunk_item_containers(chunk_index);

}

void Map_manager::unsub(int alias)
{
    int chunk_index = alias_list[alias];


    if(version_list[chunk_index].version == QUED)
    {
        printf("Error: Map_manager::unsub, unsubbing element on que!? wtf\n");
        return;
    }
    //printf("unsub: %i %i \n", index % xchunk_dim, index / xchunk_dim);
    version_list[chunk_index].version = map_history->chunk[chunk_index].version; //cache version

    map_history->chunk[chunk_index].remove_subscriber(client_id);
    send_chunk_item_container_reset(chunk_index);

    alias_list[alias] = NO_ALIAS;
    subed_chunks--;

    //send unsubscribe message
}


}   // t_map

