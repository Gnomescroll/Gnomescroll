#include "map.hpp"

#include <common/files.hpp>
#include <serializer/_interface.hpp>
#include <serializer/_state.hpp>

namespace serializer
{

static const size_t CHUNK_SIZE = CHUNK_ELEMENT_COUNT * sizeof(struct t_map::MAP_ELEMENT);

static int* cube_id_map = NULL;

bool should_save_world = false;
BlockSerializer* block_serializer = NULL;

static bool map_save_completed = false;
bool map_save_memcpy_in_progress = false;

void init_map_serializer()
{
    GS_ASSERT(block_serializer == NULL);
    block_serializer = new BlockSerializer;

    GS_ASSERT(cube_id_map == NULL);
    cube_id_map = (int*)malloc(MAX_CUBES * sizeof(int));
    for (int i=0; i<MAX_CUBES; cube_id_map[i++] = -1);
}

void teardown_map_serializer()
{
    if (block_serializer != NULL) delete block_serializer;
    if (cube_id_map != NULL) free(cube_id_map);
}

#if PTHREADS_ENABLED
//threaded IO

struct ThreadedWriteData
{
    char filename[NAME_MAX+1];
    char* buffer;
    int buffer_size;
};

static bool _threaded_write_running = false;
static struct ThreadedWriteData threaded_write_data;
static pthread_t map_save_thread = 0;

void* _threaded_write(void* vptr)
{
    int ti1 = _GET_MS_TIME();

    char filename[NAME_MAX+1] = {'\0'}; 
    strcpy(filename, threaded_write_data.filename);
    char* buffer = threaded_write_data.buffer;
    int buffer_size = threaded_write_data.buffer_size;


    if (buffer == NULL)
    {
        printf("ERROR _threaded_write: t->buffer is NULL!\n");
        return NULL;
    }

    FILE *file; 
    file = fopen(filename, "wb+");
    
    if (file == 0)
    {
        printf("THREAD WRITE ERROR: cannot open map file %s \n", filename);
        return NULL;
    }

    int ret = fwrite(buffer, sizeof(char), buffer_size, file);
    GS_ASSERT(ret == buffer_size);
    ret = fclose(file); /*done!*/
    GS_ASSERT(ret == 0);

    free(buffer);

    int ti2 = _GET_MS_TIME();

    printf("_threaded_write: map saved to %s in %i ms \n", filename, ti2-ti1);

    map_save_completed = true;
    _threaded_write_running = false;

    threaded_write_data.filename[0] = '\0';
    threaded_write_data.buffer = NULL;
    threaded_write_data.buffer_size = 0;

    return NULL;
}

static void threaded_write(const char* filename, char* buffer, int buffer_len)
{
    if (_threaded_write_running != 0)
    {
        printf("threaded_write failed: previous thread has not finished \n");
        return;
    }

    strcpy(threaded_write_data.filename, filename);
    threaded_write_data.buffer = buffer;
    threaded_write_data.buffer_size = buffer_len;

    //pthread_join( map_save_thread, NULL);
    /* Create independent threads each of which will execute function */

    _threaded_write_running = true;

    int ret = pthread_create( &map_save_thread, NULL, _threaded_write, (void*)NULL);
    if (ret != 0)
    {
        printf("threaded_write error: pthread_create returned %i \n", ret);
        _threaded_write_running = false;
    }
    else
        printf("Thread created\n");
}

void wait_for_threads()
{
    GS_ASSERT(!_threaded_write_running || map_save_thread != 0);
    if (!_threaded_write_running) return;

    GS_ASSERT(map_save_thread != 0);
    printf("Waiting for threads to finish...\n");
    pthread_join(map_save_thread, NULL);
    pthread_detach(map_save_thread);
    printf("Thread detached\n");
    map_save_thread = 0;
}

bool save_map_iter(int max_ms)
{
    return block_serializer->save_iter(2);
}
#endif

static bool parse_map_palette_token(const char* key, const char* val, class ParsedMapPaletteData* data)
{
    bool err = false;
    if (strcmp(CUBE_ID_TAG, key) == 0)
    {
        long long cube_id = parse_int(val, err);
        ASSERT_VALID_CUBE_ID(cube_id);
        IF_INVALID_CUBE_ID(cube_id) return false;
        GS_ASSERT(!err);
        if (err) return false;
        data->cube_id = (int)cube_id;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = t_map::is_valid_cube_name(val);
        GS_ASSERT(valid_name);
        if (!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }

    return true;
}

bool load_map_palette_file(const char* fn)
{
    printf("Loading map palette file %s\n", fn);

    size_t size = 0;
    char* str = read_file_to_buffer(fn, &size);
    GS_ASSERT(str != NULL)
    GS_ASSERT(size > 0);
    if (str == NULL) return false;
    if (size <= 0)
    {
        free(str);
        return false;
    }

    // allocate scratch buffer long enough to hold the largest line
    static const size_t LONGEST_LINE = MAP_PALETTE_LINE_LENGTH;
    char buf[LONGEST_LINE+1] = {'\0'};

    size_t istr = 0;
    class ParsedMapPaletteData palette_data;
    while (istr < size)
    {    
        // copy line
        size_t ibuf = 0;
        char c = '\0';
        while ((c = str[istr++]) != '\0' && c != '\n' && ibuf < LONGEST_LINE)
            buf[ibuf++] = c;
        buf[ibuf] = '\0';
        GS_ASSERT(c == '\n' || c == '\0');
        if (c != '\0' && c != '\n')
        {
            free(str);
            return false;
        }

        parse_line<class ParsedMapPaletteData>(&parse_map_palette_token, buf, ibuf, &palette_data);
        GS_ASSERT(palette_data.valid);
        if (!palette_data.valid)
        {
            free(str);
            return false;
        }

        const char* actual_name = t_map::get_compatible_cube_name(palette_data.name);
        GS_ASSERT(actual_name != NULL);
        if (actual_name == NULL)
        {   // we failed to get a compatible cube name
            free(str);
            return false;
        }
        CubeID actual_cube_id = t_map::get_cube_id(actual_name);
        GS_ASSERT(t_map::isInUse(actual_cube_id));
        if (!t_map::isInUse(actual_cube_id))
        {   // we failed to get a compatible block type
            free(str);
            return false;
        }
        GS_ASSERT(cube_id_map[palette_data.cube_id] < 0);
        if (cube_id_map[palette_data.cube_id] >= 0)
        {   // duplicate entry
            free(str);
            return false;
        }
        cube_id_map[palette_data.cube_id] = actual_cube_id;
        
        if (c == '\0') break;
    }

    free(str);

    return true;
}

bool save_map_palette_file()
{
    FILE* f = fopen(map_palette_path_tmp, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL)
    {
        log_map_save_error("Saving map palette file: failed to open file");
        return false;
    }

    char buf[MAP_PALETTE_LINE_LENGTH+2] = {'\0'};

    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!t_map::isInUse((CubeID)i)) continue; 
        const char* cube_name = t_map::get_cube_name((CubeID)i);
        if (cube_name == NULL) continue;
        int len = snprintf(buf, MAP_PALETTE_LINE_LENGTH+1, MAP_PALETTE_FMT, cube_name, i);
        GS_ASSERT(len >= 0 && (size_t)len < MAP_PALETTE_LINE_LENGTH+1);
        if (len < 0 || (size_t)len >= MAP_PALETTE_LINE_LENGTH+1)
        {
            log_map_save_error("Saving map palette file: buffer overrun");
            return false;
        }
        buf[len++] = '\n';
        buf[len] = '\0';
        
        size_t wrote = fwrite(buf, sizeof(char), (size_t)len, f);
        GS_ASSERT(wrote == (size_t)len);
        if (wrote != (size_t)len)
        {
            log_map_save_error("Saving map palette file: failed to write to file");
            return false;
        }
    }

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    return (ret == 0);
}

static bool load_map_restore_containers()
{
    for (int ci=0; ci < MAP_CHUNK_XDIM; ci++)
    for (int cj=0; cj < MAP_CHUNK_YDIM; cj++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[MAP_CHUNK_XDIM*cj+ci];
        IF_ASSERT(mp == NULL) continue;

        for (int k=0; k<ZMAX; k++)
        for (int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
        for (int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
        {
            CubeID block = (CubeID)mp->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*k + TERRAIN_CHUNK_WIDTH*j + i].block;
            if (!t_map::isItemContainer(block)) continue;
            
            ItemContainerType container_type = t_map::get_container_type_for_cube(block);

            IF_ASSERT(container_type == NULL_CONTAINER_TYPE)
            {
                log_map_load_error("Restoring containers from map: Container type is NULL_CONTAINER_TYPE");
                return false;
            }

            class ItemContainer::ItemContainerInterface* container = ItemContainer::create_container(container_type);
            IF_ASSERT(container == NULL)
            {
                log_map_load_error("Restoring containers from map: Container creation failed");
                return false;
            }

            t_map::create_item_container_block(ci*TERRAIN_CHUNK_WIDTH+i, cj*TERRAIN_CHUNK_WIDTH+j, k, container->type, container->id);
            loaded_containers[container->id] = CONTAINER_LOAD_MAP;
        }
    }
    return true;
}

BlockSerializer::BlockSerializer()
{
    this->filename[0] = '\0';
    this->write_buffer = NULL;
    this->file_size = 0;

    this->chunk = (struct SerializedChunk*) malloc(sizeof(struct SerializedChunk));

    #if PTHREADS_ENABLED
    for (int i=0; i<CHUNK_COUNT; this->version_array[i++] = -1);
    #endif
}

BlockSerializer::~BlockSerializer()
{
    if (this->write_buffer != NULL) free(this->write_buffer);
    if (this->chunk != NULL) free(this->chunk);
}

bool BlockSerializer::save(const char* filename)
{
    GS_ASSERT(!map_save_memcpy_in_progress);
    if (map_save_memcpy_in_progress)
    {
        printf("BlockSerializer::save call failed, map memcpy already in progress\n");
        return false;
    }

    #if PTHREADS_ENABLED
    GS_ASSERT(!_threaded_write_running);
    if (_threaded_write_running)
    {
        printf("BlockSerializer::save call failed; threaded write in progress\n");
        return false;
    }
    #endif

    map_save_memcpy_in_progress = true;

    GS_ASSERT(this->write_buffer == NULL);
    GS_ASSERT(this->filename[0] == '\0');
    GS_ASSERT(file_size == 0);

    strcpy(this->filename, filename);

    this->file_size = prefix_length + CHUNK_COUNT*sizeof(struct SerializedChunk);
    if (this->write_buffer == NULL)
        this->write_buffer = (char*)calloc(file_size, sizeof(char));
    else
        memset(this->write_buffer, file_size, sizeof(char));

    GS_ASSERT(this->write_buffer != NULL);
    if (this->write_buffer == NULL)
    {
        printf("BlockSerializer: cannot save map.  malloc failed, out of memory? \n");
        return false;
    }

    //push header
    int index = 0;
    push_int(this->write_buffer, index, version);

    int ti1 = _GET_MS_TIME();

    //serialize
    #if PTHREADS_ENABLED
    for (int i=0; i<CHUNK_COUNT; this->version_array[i++] = -1);
    // iterated save data copying should be done in the main server loop
    #else
    for (int i=0; i < CHUNK_COUNT; i++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[i];
        GS_ASSERT(mp != NULL);
        this->chunks[i].xchunk = CHUNK_COUNT % 16;
        this->chunks[i].ychunk = CHUNK_COUNT / 16;
        memcpy((void*) &this->chunks[i].data, &mp->e, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
    }
    //prepare buffer for saving

    for (int i=0; i<CHUNK_COUNT; i++)
    {
        memcpy(&this->write_buffer[index], (char*) &this->chunks[i], sizeof(struct SerializedChunk));
        index += sizeof(struct SerializedChunk);
    }
    GS_ASSERT(file_size == (size_t)index);

    map_save_memcpy_in_progress = false;
    #endif

    int ti2 = _GET_MS_TIME();

    //int ti3 = _GET_MS_TIME();
    printf("BlockSerializer save: memcpy buffer for %s  took %i ms \n", filename, ti2-ti1);

    return true;
}

#if PTHREADS_ENABLED
//this is called until map is done saving
//will memcpy map and yield after ms milliseconds
bool BlockSerializer::save_iter(int max_ms)
{
    if (!map_save_memcpy_in_progress) return true;
    
    static int _start_ms = 0;
    static int _calls = 0;
    static int _memcpy_count = 0;

    _calls++;
    if (_start_ms == 0)
       _start_ms = _GET_MS_TIME();
    int start_ms = _GET_MS_TIME();

    static int index = 0;

    for (int j=0; j < CHUNK_COUNT; j++)
    {
        index = (index+1)%CHUNK_COUNT;
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[index];
        GS_ASSERT(mp != NULL);
        if (mp == NULL) continue;
        if (mp->version == version_array[index]) continue;
        chunk->xchunk = CHUNK_COUNT % TERRAIN_CHUNK_WIDTH;
        chunk->ychunk = CHUNK_COUNT / TERRAIN_CHUNK_WIDTH;
        memcpy((void*) &chunk->data, &mp->e, CHUNK_SIZE);
        version_array[index] = mp->version;

        int write_index = prefix_length+index*sizeof(struct SerializedChunk);
        memcpy(write_buffer+write_index, (void*) chunk, CHUNK_SIZE);
        _memcpy_count++;

        int _ctime = _GET_MS_TIME();

        if ( _ctime > start_ms + max_ms || abs(_ctime - start_ms) > 1000)
            return false; //yield after n ms
    }

    threaded_write(this->filename, write_buffer, file_size);

    _start_ms = 0;
    _calls = 0;
    _memcpy_count=0;

    map_save_memcpy_in_progress = false;
    write_buffer = NULL;
    this->filename[0] = '\0';
    file_size = 0;

    return true;
}
#endif

bool BlockSerializer::load(const char* filename)
{
    printf("Loading map: %s\n", filename);
        
    GS_ASSERT(t_map::main_map != NULL);

    GS_ASSERT(filename != NULL);
    if (filename == NULL) return false;

    int ti1 = _GET_MS_TIME();
    size_t filesize = 0;
    char* buffer = read_binary_file_to_buffer(filename, &filesize);
    GS_ASSERT(buffer != NULL);
    if (buffer == NULL) return false;

    int ti2 = _GET_MS_TIME();

    int _version = 0; 
    int index = 0;
    pop_int(buffer, index, _version);

    printf("Build Version: %d; Filesize: %lu\n", _version, (unsigned long)filesize);

    size_t expected_filesize = prefix_length + CHUNK_COUNT*sizeof(struct SerializedChunk);
    if (filesize != expected_filesize)
        printf("WARNING: Map filesize %lu does not match expected filesize %lu\n", (unsigned long)filesize, (unsigned long)expected_filesize); 
    GS_ASSERT(filesize == expected_filesize);

    // apply block id versioning transformation
    for (size_t i=0; i<filesize; i++)
    {
        char c = buffer[i];
        ASSERT_VALID_CUBE_ID(c);
        IF_INVALID_CUBE_ID(c)
            c = (char)EMPTY_CUBE;
        buffer[i] = cube_id_map[(unsigned char)c];
    }

    for (int i=0; i<CHUNK_COUNT; i++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[i];
        GS_ASSERT(mp != NULL);
        if (mp == NULL) continue;

        memcpy((char*) chunk, buffer+index, sizeof(struct SerializedChunk));
        index += sizeof(struct SerializedChunk);
        GS_ASSERT(index == (prefix_length + (i+1)*(int)sizeof(struct SerializedChunk)));
        memcpy(&mp->e, (void*) &chunk->data, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
    }

    int ti3 = _GET_MS_TIME();

    printf("BlockSerializer load: reading map file %s took %i ms \n", filename, ti2-ti1);
    printf("BlockSerializer load: loading map file %i ms \n", ti3-ti2);

    free(buffer);

    return load_map_restore_containers();  //setup containers
}

bool save_map()
{
    GS_ASSERT(!map_save_memcpy_in_progress);
    if (map_save_memcpy_in_progress) return false;
    #if PTHREADS_ENABLED
    GS_ASSERT(!_threaded_write_running);
    if (_threaded_write_running) return false;
    #endif
    
    GS_ASSERT(block_serializer != NULL);
    if (block_serializer == NULL) return false;
    create_path_to_file(map_path_tmp);
    return block_serializer->save(map_path_tmp);        
}

bool load_map()
{
    if (file_exists(map_path) && fsize(map_path) > 0)
    {
        if (!load_map_palette_file(map_palette_path)) return false;
        return block_serializer->load(map_path);
    }
    else
    if (file_exists(map_path_bak) && fsize(map_path_bak) > 0)
    {
        if (!load_map_palette_file(map_palette_path_bak)) return false;
        return block_serializer->load(map_path_bak);
    }
    return false;
}

void update_completed_map_save()
{
    if (!map_save_completed) return;
    bool saved = save_tmp_file(map_path, map_path_tmp, map_path_bak);
    GS_ASSERT(saved);
    map_save_completed = false; // reset
    GS_ASSERT(!map_save_memcpy_in_progress);

    #if PTHREADS_ENABLED
    GS_ASSERT(!_threaded_write_running);

    if (!_threaded_write_running)
    {
        if (map_save_thread != 0)
        {
            pthread_detach(map_save_thread);
            printf("Thread detached\n");
        }
        map_save_thread = 0;
    }
    #endif
}

}   // serializer
