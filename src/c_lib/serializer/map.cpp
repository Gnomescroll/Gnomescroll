#include "map.hpp"

#include <common/files.hpp>
#include <serializer/_interface.hpp>
#include <serializer/_state.hpp>

namespace serializer
{

static int* cube_id_map = NULL;

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
    char filename[256];
    char* buffer;
    int buffer_size;
};

static int _threaded_write_running = 0;
static struct ThreadedWriteData threaded_write_data;
static pthread_t _threaded_write_thread;

void* _threaded_write(void* vptr)
{
    int ti1 = _GET_MS_TIME();

    char filename[256]; 
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
    _threaded_write_running = 0;

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

    //pthread_join( _threaded_write_thread, NULL);
    /* Create independent threads each of which will execute function */

    _threaded_write_running = 1;

    int ret = pthread_create( &_threaded_write_thread, NULL, _threaded_write, (void*)NULL);
    if (ret != 0)
    {
        printf("threaded_write error: pthread_create returned %i \n", ret);
        _threaded_write_running = 0;
    }
}

void wait_for_threads()
{
    while (_threaded_write_running != 0)
        gs_microsleep(100);
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
        strncpy(data->name, val, CUBE_NAME_MAX_LENGTH);
        data->name[CUBE_NAME_MAX_LENGTH] = '\0';
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

        CubeID actual_cube_id = t_map::get_compatible_cube_id(palette_data.name);
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
    FILE* f = fopen(map_palette_filename_tmp, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL) return false;

    char buf[MAP_PALETTE_LINE_LENGTH+2] = {'\0'};

    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!t_map::isInUse((CubeID)i)) continue; 
        const char* cube_name = t_map::get_cube_name((CubeID)i);
        if (cube_name == NULL) continue;
        int len = snprintf(buf, MAP_PALETTE_LINE_LENGTH+1, MAP_PALETTE_FMT, cube_name, i);
        GS_ASSERT(len >= 0 && (size_t)len < MAP_PALETTE_LINE_LENGTH+1);
        if (len < 0 || (size_t)len >= MAP_PALETTE_LINE_LENGTH+1) return false;
        buf[len++] = '\n';
        buf[len] = '\0';
        
        size_t wrote = fwrite(buf, sizeof(char), (size_t)len, f);
        GS_ASSERT(wrote == (size_t)len);
        if (wrote != (size_t)len) return false;
    }

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    if (ret != 0) return false;

    return save_file(map_palette_filename, map_palette_filename_tmp, map_palette_filename_backup);
}

static void load_map_restore_containers()
{
    for (int ci=0; ci < 32; ci++)
    for (int cj=0; cj < 32; cj++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[32*cj+ci];
        for (int k=0; k<128; k++)
        for (int i=0; i<16; i++)
        for (int j=0; j<16; j++)
        {
            CubeID block = (CubeID)mp->e[16*16*k + 16*j + i].block;
            if (!t_map::isItemContainer(block)) continue;
            
            ItemContainerType container_type = t_map::get_container_type_for_cube(block);
            GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
            if (container_type == CONTAINER_TYPE_NONE) continue;    // TODO -- log error
            class ItemContainer::ItemContainerInterface* container = ItemContainer::create_container(container_type);
            GS_ASSERT(container != NULL);
            if (container == NULL) continue;    // TODO -- log error
            init_container(container);            
            t_map::create_item_container_block(ci*16+i, cj*16+j, k, container->type, container->id);
            loaded_containers[container->id] = CONTAINER_LOAD_MAP;
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
    this->write_buffer = (char*)calloc(file_size, sizeof(char));

    //push header
    int index = 0;
    push_int(write_buffer, index, version);

    if (write_buffer == NULL)
    {
        printf("BlockSerializer: cannot save map.  malloc failed, out of memory? \n");
        return;
    }

    int ti1 = _GET_MS_TIME();

    //serialize
    #if PTHREADS_ENABLED
    for (int i=0; i<chunk_number; i++) version_array[i] = -1;
    while (map_save_memcpy_in_progress)
        this->save_iter(2);   //2 ms per iteration
    #else
    for (int i=0; i < chunk_number; i++)
    {
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[i];
        GS_ASSERT(mp != NULL);
        this->chunks[i].xchunk = chunk_number % 16;
        this->chunks[i].ychunk = chunk_number / 16;
        memcpy((void*) &this->chunks[i].data, &mp->e, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
    }
    //prepare buffer for saving

    for (int i=0; i<chunk_number; i++)
    {
        memcpy(&this->write_buffer[index], (char*) &this->chunks[i], sizeof(struct SerializedChunk));
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
    if (_start_ms == 0)
       _start_ms = _GET_MS_TIME();
    int start_ms = _GET_MS_TIME();


    static int index = 0;

    for (int j=0; j < chunk_number; j++)
    {
        index = (index+1)%chunk_number;
        class t_map::MAP_CHUNK* mp = t_map::main_map->chunk[index];
        if (mp->version == version_array[index]) continue;
        GS_ASSERT(mp != NULL);
        chunk->xchunk = chunk_number % 16;
        chunk->ychunk = chunk_number / 16;
        memcpy((void*) &chunk->data, &mp->e, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
        version_array[index] = mp->version;

        int write_index = prefix_length+index*sizeof(struct SerializedChunk);
        memcpy(write_buffer+write_index, (void*) chunk, 128*16*16*sizeof(struct t_map::MAP_ELEMENT));
        _memcpy_count++;

        int _ctime = _GET_MS_TIME();

        if ( _ctime > start_ms + max_ms || abs(_ctime - start_ms) > 1000)
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

bool BlockSerializer::load(const char* filename)
{
    GS_ASSERT_ABORT(t_map::main_map != NULL);

    GS_ASSERT(filename != NULL);
    if (filename == NULL) return false;

    int ti1 = _GET_MS_TIME();
    size_t filesize = 0;
    char* buffer = read_binary_file_to_buffer(filename, &filesize);
    GS_ASSERT_ABORT(buffer != NULL);
    if (buffer == NULL) return false;

    int ti2 = _GET_MS_TIME();

    int _version = 0; 
    int index = 0;
    pop_int(buffer, index, _version);

    printf("Loading map. Version: %d Filesize: %d\n", _version, filesize);
    if (_version != version)
        printf("WARNING: Map version %d does not match build version %d\n", _version, version);
    GS_ASSERT_ABORT(_version == version);

    size_t expected_filesize = prefix_length + chunk_number*sizeof(struct SerializedChunk);
    if (filesize != expected_filesize)
        printf("WARNING: Map filesize %u does not match expected filesize %u\n", filesize, expected_filesize); 
    GS_ASSERT_ABORT(filesize == expected_filesize);

    // apply block id versioning transformation
    for (size_t i=0; i<filesize; i++)
    {
        char c = buffer[i];
        ASSERT_VALID_CUBE_ID(c);
        IF_INVALID_CUBE_ID(c)
        {
            free(buffer);
            GS_ABORT();
        }
        buffer[i] = cube_id_map[(unsigned char)c];
    }

    for (int i=0; i<chunk_number; i++)
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

    load_map_restore_containers();  //setup containers

    return true;
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
        const char fmt[] = "%s" DATA_TMP_EXT;
        char* tmp_filename = (char*)malloc((strlen(filename) + sizeof(fmt) - 2)*sizeof(char));
        sprintf(tmp_filename, fmt, filename);
        map_tmp_name = tmp_filename;
        block_serializer->save(tmp_filename);        
    }
    else
        block_serializer->save(filename);
}


bool load_map(const char* filename)
{
    GS_ASSERT(block_serializer != NULL);
    if (block_serializer == NULL) return false;
    return block_serializer->load(filename);
}

void save_map()
{
    save_map(map_filename);
}

bool load_map()
{
    return load_map(map_filename);
}

bool load_default_map()
{
    if (file_exists(map_filename) && fsize(map_filename) > 0)
    {
        if (!load_map_palette_file(map_palette_filename)) return false;
        return load_map(map_filename);
    }
    else
    if (file_exists(map_filename_backup) && fsize(map_filename_backup) > 0)
    {
        if (!load_map_palette_file(map_palette_filename_backup)) return false;
        return load_map(map_filename_backup);
    }
    return false;
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
                    const char fmt[] = "%s" DATA_BACKUP_EXT;
                    char* map_final_name_bak = (char*)malloc((strlen(map_final_name) + sizeof(fmt) - 2)*sizeof(char));
                    sprintf(map_final_name_bak, fmt, map_final_name);
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
