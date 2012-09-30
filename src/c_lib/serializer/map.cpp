#include "map.hpp"

#include <common/files.hpp>

namespace serializer
{

bool should_save_map = false;
BlockSerializer* block_serializer = NULL;

bool map_save_completed = false;
char* map_tmp_name = NULL;
char* map_final_name = NULL;
bool map_save_memcpy_in_progress = false;

void init_map_serializer()
{
    GS_ASSERT(block_serializer == NULL);
    block_serializer = new BlockSerializer;
}

void teardown_map_serializer()
{
    if (block_serializer != NULL) delete block_serializer;
}

#if PTHREADS_ENABLED
//threaded IO

struct THREADED_WRITE_STRUCT
{
    char filename[256];
    char* buffer;
    int buffer_size;
};

static int _threaded_write_running = 0;
static struct THREADED_WRITE_STRUCT threaded_write_struct_param;
static pthread_t _threaded_write_thread;

void* _threaded_write(void* vptr)
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

    threaded_write_struct_param.filename[0] = '\0';
    threaded_write_struct_param.buffer = NULL;
    threaded_write_struct_param.buffer_size = 0;

    return NULL;
}

static void threaded_write(const char* filename, char* buffer, int buffer_len)
{
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
}

void wait_for_threads()
{
    while(_threaded_write_running != 0)
        gs_microsleep(100);
}
#endif

static void load_map_restore_containers()
{
    for(int ci=0; ci < 32; ci++)
    for(int cj=0; cj < 32; cj++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[32*cj+ci];
        for(int k=0; k<128; k++)
        for(int i=0; i<16; i++)
        for(int j=0; j<16; j++)
        {
            int block = mp->e[16*16*k + 16*j + i].block;
            if(isItemContainer(block))
                t_map::load_item_container_block(ci*16+i, cj*16+j, k, block);
        }
    }
}

BlockSerializer::BlockSerializer()
{
    this->file_name[0] = '\0';
    this->write_buffer = NULL;
    this->file_size = 0;

    this->chunk = (struct SerializedChunk*) malloc(sizeof(struct SerializedChunk));

    #if PTHREADS_ENABLED
    this->version_array = (int*) malloc(chunk_number*sizeof(int));
    #else
    this->chunks = (struct SerializedChunk*)malloc(chunk_number * sizeof(struct SerializedChunk));
    #endif 
}

BlockSerializer::~BlockSerializer()
{
    if (this->chunk != NULL) free(this->chunk);

    #if PTHREADS_ENABLED
    if (this->version_array != NULL) free(this->version_array);
    #else
    if (this->chunks != NULL) free(this->chunks);
    #endif
}

void BlockSerializer::save(const char* filename)
{
    if (map_save_memcpy_in_progress)
    {
        printf("BlockSerializer::save call failed, map memcpy already in progress \n");
        return; 
    }

    map_save_memcpy_in_progress = true;

    GS_ASSERT(write_buffer == NULL);
    GS_ASSERT(file_name[0] == '\0');
    GS_ASSERT(file_size == 0);

    strcpy(file_name, filename);

    this->file_size = prefix_length + chunk_number*sizeof(struct SerializedChunk);
    this->write_buffer = (char*) malloc(file_size);

    //push header
    int index = 0;
    push_int(write_buffer, index, version);

    if(write_buffer == NULL)
    {
        printf("BlockSerializer: cannot save map.  malloc failed, out of memory? \n");
        return;
    }


    int ti1 = _GET_MS_TIME();

    //serialize
    #if PTHREADS_ENABLED
    for(int i=0; i<chunk_number; i++) version_array[i] = -1;
    while (map_save_memcpy_in_progress)
        this->save_iter(2);   //2 ms per iteration
    #else
    for(int i=0; i < chunk_number; i++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[i];
        GS_ASSERT(mp != NULL);
        s[i].xchunk = chunk_number % 16;
        s[i].ychunk = chunk_number / 16;
        memcpy((void*) &s[i].data, &mp->e, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
    }
    //prepare buffer for saving

    for(int i=0; i<chunk_number; i++)
    {
        memcpy(&this->write_buffer[index], (char*) &s[i], sizeof(struct SerializedChunk));
        index += sizeof(struct SerializedChunk);
    }
    GS_ASSERT(file_size == (size_t)index);
    #endif

    int ti2 = _GET_MS_TIME();

    //int ti3 = _GET_MS_TIME();
    printf("BlockSerializer save: memcpy buffer for %s  took %i ms \n", filename, ti2-ti1);
}

#if PTHREADS_ENABLED
//this is called until map is done saving
//will memcpy map and yield after ms milliseconds
void BlockSerializer::save_iter(int max_ms)
{

    static int _start_ms = 0;
    static int _calls = 0;
    static int _memcpy_count = 0;

    _calls++;
    if(_start_ms == 0)
       _start_ms = _GET_MS_TIME();
    int start_ms = _GET_MS_TIME();


    static int index = 0;

    for(int j=0; j < chunk_number; j++)
    {
        index = (index+1)%chunk_number;
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[index];
        if(mp->version == version_array[index]) continue;
        GS_ASSERT(mp != NULL);
        chunk->xchunk = chunk_number % 16;
        chunk->ychunk = chunk_number / 16;
        memcpy((void*) &chunk->data, &mp->e, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
        version_array[index] = mp->version;

        int write_index = prefix_length+index*sizeof(struct SerializedChunk);
        memcpy(write_buffer+write_index, (void*) chunk, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
        _memcpy_count++;

        int _ctime = _GET_MS_TIME();

        if( _ctime > start_ms + max_ms || abs(_ctime - start_ms) > 1000)
            return; //yield after n ms
    }

    printf("BlockSerializer save_itr complete: clock_time= %i ms num_calls= %i, ms_per_call= %i, chunks_per_call= %i total_chunks= %i memcpy_count= %i \n", 
        _GET_MS_TIME()-_start_ms, _calls, max_ms, chunk_number/_calls, chunk_number, _memcpy_count);

    threaded_write(this->file_name, write_buffer, file_size);

    _start_ms = 0;
    _calls = 0;
    _memcpy_count=0;

    map_save_memcpy_in_progress = false;
    write_buffer = NULL;
    file_name[0] = '\0';
    file_size = 0;
}
#endif

void BlockSerializer::load(const char* filename)
{
    GS_ASSERT(filename != NULL);
    if (filename == NULL) return;
    
    if(t_map::main_map == NULL)
    {
        printf("ERROR: Attempting to load map before t_map init \n");
        GS_ABORT();
    }

    int ti1 = _GET_MS_TIME();
    int filesize;
    char* buffer = fast_read_file_to_buffer(filename, &filesize);
    if(buffer == NULL) GS_ABORT();

    int ti2 = _GET_MS_TIME();

    int _version = 0; 
    int index = 0;
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
        return;
    }

    for(int i=0; i<chunk_number; i++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[i];
        GS_ASSERT(mp != NULL);
        if(mp == NULL) continue;

        memcpy((char*) chunk, buffer+index, sizeof(struct SerializedChunk) );
        index += sizeof(struct SerializedChunk);
        GS_ASSERT(index == (prefix_length + (i+1)*(int)sizeof(struct SerializedChunk)));
        memcpy(&mp->e, (void*) &chunk->data, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
    }

    int ti3 = _GET_MS_TIME();

    printf("BlockSerializer load: reading map file %s took %i ms \n", filename, ti2-ti1);
    printf("BlockSerializer load: loading map file %i ms \n", ti3-ti2);

    free(buffer);

    load_map_restore_containers();  //setup containers
}

void save_map(const char* filename)
{
    GS_ASSERT(block_serializer != NULL);
    if (block_serializer == NULL) return;
    
    create_path_to_file(filename);

    map_final_name = (char*)malloc((strlen(filename)+1)*sizeof(char));
    strcpy(map_final_name, filename);
    
    if (file_exists(filename))
    {
        const char ext[] = ".tmp";
        char* tmp_filename = (char*)malloc((strlen(filename) + sizeof(ext))*sizeof(char));
        sprintf(tmp_filename, "%s%s", filename, ext);
        map_tmp_name = tmp_filename;
        block_serializer->save(tmp_filename);        
    }
    else
        block_serializer->save(filename);
}


void load_map(const char* filename)
{
    GS_ASSERT(block_serializer != NULL);
    if (block_serializer == NULL) return;
    block_serializer->load(filename);
}

void save_map()
{
    save_map(default_map_file);
}

void load_map()
{
    load_map(default_map_file);
}

void check_map_save_state()
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

}   // serializer
