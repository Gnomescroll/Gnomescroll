#pragma once

#include <c_lib/t_map/t_map_class.hpp>
#include <c_lib/t_map/t_map.hpp>

#include <c_lib/t_map/net/t_StoC.hpp>

#include <c_lib/common/qsort.h>

namespace t_map
{

const int COMPRESSION_BUFFER_SIZE = 1024*512;
extern char* COMPRESSION_BUFFER;

const int  DEFAULT_MAP_MANAGER_RADIUS = 32;
const int UNSUB_DISTANCE = 40;
const int UNSUB_DISTANCE2 = UNSUB_DISTANCE*UNSUB_DISTANCE;

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
    int radius;

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

        radius = DEFAULT_MAP_MANAGER_RADIUS;

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
        delete[] version_list;
    }

    void update();
    void set_position(int x, int y);

    void sort_que();
    void dispatch_que();

    private:

    void que_for_sub(int x, int y);

    void sub(int index, int version);
    void unsub(int alias);

    void send_compressed_chunk(int alias, int index);
    void send_uncompressed_chunk(int alias, int index);

    void init_compressor();

    void send_delta() {}
};


void Map_manager::init_compressor()
{
    //#define BUF_SIZE (1024 * 1024)

    //uint infile_size;
    int level = MZ_BEST_COMPRESSION;

    // Init the z_stream
    memset(&stream, 0, sizeof(stream));  

    if (mz_deflateInit(&stream, level) != MZ_OK)
    {
        printf("deflateInit() failed!\n");
        return;
    }
}

void Map_manager::send_compressed_chunk(int alias, int index)
{

    if(t->chunk[index] == NULL)
    {
        printf("Chunk is null!  Handle this! \n");
        return;
    }

    stream.next_in = (unsigned char*) t->chunk[index]->e;
    stream.avail_in = 4*16*16*128;

    stream.next_out = (unsigned char*) COMPRESSION_BUFFER;
    stream.avail_out = COMPRESSION_BUFFER_SIZE;  

    int status;

    status = mz_deflate(&stream, MZ_SYNC_FLUSH); //Z_FINISH

/*
    if ( status == MZ_STREAM_END )
    {
    // Output buffer is full, or compression is done
    uint n = BUF_SIZE - stream.avail_out;  //number of bytes written
    stream.next_out = s_outbuf;   //reset output buffer
    stream.avail_out = BUF_SIZE;  //reset output buffer
    }
*/
    if (status != MZ_OK)
    {
        printf("Map_manager::send_compressed_chunk: deflate() failed with status %i! \n", status);
        return;
    }

    int size = COMPRESSION_BUFFER_SIZE - stream.avail_out;  //number of bytes written

    printf("compressed size = %i \n", size);

    map_chunk_compressed_StoC c;

    c.chunk_alias = alias;
    c.chunk_index = index;
    //c.byte_size = 4*16*16*128;
    //int size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;
    //c.byte_size = size;
    c.sendToClient(client_id, COMPRESSION_BUFFER, size);

}

void Map_manager::update()
{

    if(needs_update == false) return;

    int imin = (xpos - radius) > 0 ? xpos - radius : 0;
    int jmin = (ypos - radius) > 0 ? ypos - radius : 0;

    int imax = (xpos+radius) < xchunk_dim ? xpos+radius : xchunk_dim;
    int jmax = (ypos+radius) < ychunk_dim ? ypos+radius : ychunk_dim;


    for(int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
    {
        if( alias_list[i] == NO_ALIAS) continue;  //QUED || NO_ALIAS 
        int x = (alias_list[i] % xchunk_dim);
        int y = (alias_list[i] / xchunk_dim);

        x = x - xpos;
        y = y - ypos; 

        if( x*x + y*y > UNSUB_DISTANCE2 ) unsub(i);
    }

    for(int i=imin;i<imax; i++)
    for(int j=jmin;j<jmax; j++)
    {
        if( (xpos - i)*(xpos - i) + (ypos - j)*(ypos - j) >= radius ) continue;
        unsigned int version = version_list[j*xchunk_dim + i].version;

        if( version == SUBSCRIBED || version == QUED ) continue;
        //printf("sub %i %i \n", i,j);
        que_for_sub(i,j);
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

    {
        int _x = xpos -x;
        int _y = ypos -y;
        q.distance2 = _x*_x + _y*_y;
    }
    q.index = index;

    chunk_que[chunk_que_num] = q;
    chunk_que_num++;
    //set version, so it is not requed
    version_list[index].version = QUED; //set subscription property, so it does not get requed
}

static inline void QUE_ELEMENT_qsort(struct QUE_ELEMENT *arr, unsigned n) 
{
    #define QUE_ELEMENT_lt(a,b) ((a)->distance2 > (b)->distance2)
    QSORT(struct QUE_ELEMENT, arr, n, QUE_ELEMENT_lt );
}

void Map_manager::sort_que()
{
    if(chunk_que_num == 0) return;
    QUE_ELEMENT_qsort(chunk_que, chunk_que_num);
}

void Map_manager::dispatch_que()
{
    if( chunk_que_num == 0) return;
    struct QUE_ELEMENT* q = &chunk_que[chunk_que_num-1];
    sub(q->index, q->version);
    chunk_que_num--;
}




void Map_manager::send_uncompressed_chunk(int alias, int index)
{
    if(t->chunk[index] == NULL)
    {
        printf("Chunk is null!  Handle this! \n");
        return;
    }
    map_chunk_uncompressed_StoC c;

    c.chunk_alias = alias;
    c.chunk_index = index;

    int size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;
    //c.byte_size = size;
    c.sendToClient(client_id, (char*)t->chunk[index]->e, size);

    //void sendToClient(int client_id, char* buff, int len) 
    //map_chunk_uncompressed_StoC
}

//this is chunk position!
void Map_manager::set_position(int x, int y)
{
    if(x != xpos || y != ypos) needs_update = true;
    xpos = x;
    ypos = y;      
}

void Map_manager::sub(int index, int version)
{
    //printf("sub: %i %i \n", x, y);
    //int index = y*xchunk_dim + x;
    //int version = version_list[index].version;
    if( subed_chunks == MAP_MANAGER_ALIAS_LIST_SIZE)
    {
        printf("FIX THIS!!! Map_manager::sub, alias list maxed out \n");
        return;
    }

    int alias=0;
    while( alias_list[alias] != NO_ALIAS) alias++;
    //set alias
    alias_list[alias] = index;

    map_history->chunk[index].add_subscriber(client_id, alias, version_list[index].version);

    version_list[index].version = SUBSCRIBED;
    subed_chunks++;

    //send_uncompressed_chunk(alias, index);

    send_compressed_chunk(alias, index);

    //send alias to client
    /*
        send alias to client
        compress map chunk
    */
}

void Map_manager::unsub(int alias)
{
    int index = alias_list[alias];


    if(version_list[index].version == QUED)
    {
        printf("Error: Map_manager::unsub, unsubbing element on que!? wtf\n");
        return;
    }


    //printf("unsub: %i %i \n", index % xchunk_dim, index / xchunk_dim);

    version_list[index].version = map_history->chunk[index].version; //cache version

    map_history->chunk[index].remove_subscriber(client_id);

    alias_list[alias] = NO_ALIAS;
    subed_chunks--;

    //send unsubscribed
}


}