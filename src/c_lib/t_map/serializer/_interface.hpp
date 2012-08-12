#pragma once


#include <common/files.hpp>

#if __GNUC__
    #include <pthread.h>
#endif

namespace t_map
{

bool map_save_completed = false;
char* map_tmp_name = NULL;
char* map_final_name = NULL;

//threaded IO
bool _THREADED_WRITE_ACTIVE;

struct THREADED_WRITE_STRUCT
{
    char filename[256];
    char* buffer;
    int buffer_size;

};

static bool map_save_memcpy_in_progress = 0;

static int _threaded_write_running = 0;
static struct THREADED_WRITE_STRUCT threaded_write_struct_param;
static pthread_t _threaded_write_thread;

void *_threaded_write(void *vptr)
{

    int ti1 = _GET_MS_TIME();


    char filename[256]; 
    strcpy(filename, threaded_write_struct_param.filename);
    char* buffer = threaded_write_struct_param.buffer;
    int buffer_size = threaded_write_struct_param.buffer_size;


    if(buffer == NULL)
    {
        printf("ERROR _threaded_write: t->buffer is NULL!\n");
        return NULL;
    }

    FILE *file; 
    file = fopen(filename, "w+"); // apend file (add text to  a file or create a file if it does not exist. 
    //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
    
    if(file == 0)
    {
        printf("THREAD WRITE ERROR: cannot open map file %s \n", filename);
        return NULL;
    }

    int ret = fwrite (buffer, buffer_size, 1, file);
    if(ret != 1)
    {
        printf("THREAD WRITE ERROR: fwrite return value != 1\n");
    }
    fclose(file); /*done!*/ 

    free(buffer);

    int ti2 = _GET_MS_TIME();

    printf("_threaded_write: map saved to %s in %i ms \n", filename, ti2-ti1);

    map_save_completed = true;
    _threaded_write_running = 0;

    threaded_write_struct_param.filename[0] = 0x00;
    threaded_write_struct_param.buffer = NULL;
    threaded_write_struct_param.buffer_size = 0;

    return NULL;
}

static
void threaded_write(const char* filename, char* buffer, int buffer_len)
{

    #if !__GNUC__
    sdfsdfjsdkjflsdkjf
    #endif

    if(_threaded_write_running != 0)
    {
        printf("threaded_write failed: previous thread has not finished \n");
        return;
    }

    strcpy(threaded_write_struct_param.filename, filename);
    threaded_write_struct_param.buffer = buffer;
    threaded_write_struct_param.buffer_size = buffer_len;

    //pthread_join( _threaded_write_thread, NULL);
    /* Create independent threads each of which will execute function */

    _threaded_write_running = 1;

    int ret = pthread_create( &_threaded_write_thread, NULL, _threaded_write, (void*)NULL);
    if(ret != 0)
    {
        printf("threaded_write error: pthread_create returned %i \n", ret);
        _threaded_write_running = 0;
    }
/*
    #if __GNUC__
    else
        printf("threaded write: __GNUC__ not defined, saving without thread \n");
        _threaded_write(NULL);
    #endif
/*/
}


void wait_for_threads()
{
    while(_threaded_write_running != 0)
    {
        #ifdef __GNUC__
        usleep(100);
        #endif
    
        #ifdef __MSVC__
        Sleep(0.1);
        #endif
    }   
}

struct SerializedChunk
{
    uint32_t xchunk;
    uint32_t ychunk;
    struct MAP_ELEMENT data[sizeof(struct MAP_ELEMENT)*16*16*128];
};

static void load_map_restore_containers()
{
    for(int ci=0; ci < 32; ci++)
    for(int cj=0; cj < 32; cj++)
    {
        class MAP_CHUNK* mp = main_map->chunk[32*cj+ci];
        for(int k=0; k<128; k++)
        for(int i=0; i<16; i++)
        for(int j=0; j<16; j++)
        {
            int block = mp->e[16*16*k + 16*j + i].block;
            if(isItemContainer(block) == true)
                load_item_container_block(ci*16+i, cj*16+j, k, block);
        }
    }
}

class BlockSerializer
{
    public:

    static const int prefix_length = sizeof(uint32_t);
    static const int version = DC_VERSION;
    static const int chunk_number = 32*32;


    char file_name[256];
    size_t file_size;
    //int blockdata_size;
    struct SerializedChunk* s; //[chunk_number]; only use for load

    struct scratch_chunk* _s;
    char* write_buffer;

    int version_array[chunk_number];

    BlockSerializer()
    {
        memset(s, 0, chunk_number*sizeof(struct SerializedChunk));
        _s = (struct scratch_chunk*) malloc(sizeof(struct scratch_chunk));
        file_name[0] = 0x00;
        write_buffer = NULL;
        file_size = 0;
    }

    ~BlockSerializer()
    {
        free(_s);
    }

    void serialize()
    {

    }

    static void push_int(char* buffer, int &index, uint32_t value)
    {
        *((uint32_t*)(buffer+index)) = value;
        index += sizeof(uint32_t);
    }

    static void pop_int(char* buffer, int &index, int &value)
    {
        value = *((uint32_t*)(buffer+index));
        index += sizeof(uint32_t);
    }

    void save(const char* filename)
    {

        if(map_save_memcpy_in_progress == true)
        {
            printf("BlockSerializer::save call failed, map memcpy already in progress \n");
            return; 
        }

        GS_ASSERT(write_buffer == NULL);
        GS_ASSERT(s == NULL);
        GS_ASSERT(strlen(file_name) == 0);
        GS_ASSERT(file_size = 0);
        for(int i=0; i<chunk_number; i++) version_array[i] = -1;

        strcpy(file_name, filename);
        //s = (struct SerializedChunk*) malloc(chunk_number* sizeof(struct SerializedChunk) ); //[chunk_number]

        file_size = prefix_length + chunk_number*sizeof(struct SerializedChunk);

        write_buffer = (char*) malloc(file_size);

        if(write_buffer == NULL)
        {
            printf("BlockSerializer: cannot save map.  malloc failed, out of memory? \n")
            return;
        }


        int ti1 = _GET_MS_TIME();
#if 0
        //serialize
        for(int i=0; i < chunk_number; i++)
        {
            class MAP_CHUNK* mp = main_map->chunk[i];
            GS_ASSERT(mp != NULL);
            s[i].xchunk = chunk_number % 16;
            s[i].ychunk = chunk_number / 16;
            memcpy((void*) &s[i].data, &mp->e, 128*16*16*sizeof(struct MAP_ELEMENT));
        }
        //prepare buffer for saving

        size_t file_size = prefix_length + chunk_number*sizeof(struct SerializedChunk);

        char* buffer = (char*) malloc(file_size*sizeof(char));
        int index = 0;

        push_int(buffer, index, version);

        for(int i=0; i<chunk_number; i++)
        {
            memcpy(&buffer[index], (char*) &s[i], sizeof(struct SerializedChunk) );
            index += sizeof(struct SerializedChunk);
        }
        GS_ASSERT(file_size == (size_t)index);
#else
        while(map_save_memcpy_in_progress == true)
            save_iter(2);   //2 ms per iteration
#endif
        int ti2 = _GET_MS_TIME();

        //int ti3 = _GET_MS_TIME();
        printf("BlockSerializer save: memcpy buffer for %s  took %i ms \n", filename, ti2-ti1);
    }

    //this is called until map is done saving
    //will memcpy map and yield after ms milliseconds
    void save_iter(int max_ms)
    {
        int start_ms = _GET_MS_TIME();

        static int index = 0;

        for(int j=0; j < chunk_number; j++)
        {
            index = (index+1)%chunk_number;
            class MAP_CHUNK* mp = main_map->chunk[index];
            if(mp->version == version_array[i]) continue;
            GS_ASSERT(mp != NULL);
            _s->xchunk = chunk_number % 16;
            _s->ychunk = chunk_number / 16;
            memcpy((void*) &s->data, &mp->e, 128*16*16*sizeof(struct MAP_ELEMENT));
            version_array[i] = mp->version;

            int write_index = prefix_length+index*sizeof(struct SerializedChunk);
            memcpy(write_buffer+write_index, (void*) _s, 128*16*16*sizeof(struct MAP_ELEMENT));

            int _ctime = _GET_MS_TIME();
            if( _ctime > start_ms + max_ms || abs(_ctime - start_ms) > 1000)
            {
                printf("chunk_memcpy: max_ms= %i memcpy= %i \n", max_ms, j);
                return; //yield after n ms
            }
        }

        printf("save_itr: completed, sending to write thread\n");


        threaded_write(this->file_name, write_buffer, file_size);

        map_save_memcpy_in_progress = false;
        write_buffer == NULL;
        file_name[0] = 0x00;
        file_size = 0;
        //free(s);
        //s = NULL;
    }

    void load(const char* filename)
    {

        if(main_map == NULL)
        {
            printf("ERROR: Attempting to load map before t_map init \n");
            GS_ABORT();
        }

        int ti1 = _GET_MS_TIME();
        int index=0 ;
        int filesize;
        char* buffer = fast_read_file_to_buffer(filename, &filesize);
        if(buffer == NULL) GS_ABORT();

        int ti2 = _GET_MS_TIME();

        int _version; 
        pop_int(buffer, index, _version);

        printf("Map Loader: map_version= %i filesize= %i bytes \n", _version, filesize);
        if(_version != version)
        {
            printf("Error: cannot load map, saved map is version %i and map loader expects %i \n", _version, version);
            GS_ABORT();
        }

        if(filesize != prefix_length + chunk_number*sizeof(struct SerializedChunk))
        {
            printf("Map Loader error: file sizes do not match!\n");
        }

        for(int i=0; i<chunk_number; i++)
        {
            class MAP_CHUNK* mp = main_map->chunk[i];
            GS_ASSERT(mp != NULL);
            if(mp == NULL) continue;

            memcpy((char*) &_s, buffer+index, sizeof(struct SerializedChunk) );
            index += sizeof(struct SerializedChunk);

            memcpy(&mp->e, (void*) &_s->data, 128*16*16*sizeof(struct MAP_ELEMENT));
        }

        int ti3 = _GET_MS_TIME();

        printf("BlockSerializer load: reading map file %s took %i ms \n", filename, ti2-ti1);
        printf("BlockSerializer load: loading map file %i ms \n", ti3-ti2);

        free(buffer);

        load_map_restore_containers();  //setup containers

        free(s);
        s = NULL;
    }
};

BlockSerializer block_serializer;

void save_map(const char* filename)
{
    create_path_to_file(filename);
    //BlockSerializer* BS = new BlockSerializer;

    map_final_name = (char*)malloc((strlen(filename)+1)*sizeof(char));
    strcpy(map_final_name, filename);
    
    if (file_exists(filename))
    {
        const char ext[] = ".tmp";
        char* tmp_filename = (char*)malloc((strlen(filename) + sizeof(ext))*sizeof(char));
        sprintf(tmp_filename, "%s%s", filename, ext);
        map_tmp_name = tmp_filename;
        block_serializer.save(tmp_filename);        
    }
    else
        block_serializer.save(filename);

    //delete BS;
}


void load_map(const char* filename)
{
    //BlockSerializer* bs = new BlockSerializer;
    block_serializer.load(filename);
    //delete bs;  
}

const char default_map_file[] = "./world/map-" STR(DC_VERSION) ".map";

void save_map()
{
    save_map(default_map_file);
}

void load_map()
{
    load_map(default_map_file);
}

void check_save_state()
{
    if (map_save_completed)
    {
        if (map_tmp_name != NULL)
        {
            GS_ASSERT(map_final_name != NULL);
            if (map_final_name != NULL)
            {
                if (file_exists(map_final_name))
                {
                    const char ext[] = ".bak";
                    char* map_final_name_bak = (char*)malloc((strlen(map_final_name) + sizeof(ext))*sizeof(char));
                    sprintf(map_final_name_bak, "%s%s", map_final_name, ext);
                    rename(map_final_name, map_final_name_bak);
                    free(map_final_name_bak);
                }
                int ret = rename(map_tmp_name, map_final_name);
                GS_ASSERT(ret == 0);
            }
            free(map_tmp_name);
            map_tmp_name = NULL;
        }
        if (map_final_name != NULL)
            free(map_final_name);
        map_final_name = NULL;
        map_save_completed = false;
    }
}

}   // t_map
