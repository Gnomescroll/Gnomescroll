/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <serializer/constants.hpp>

namespace serializer
{

struct SerializedChunk
{
    uint32_t xchunk;
    uint32_t ychunk;
    struct t_map::MapElement data[sizeof(struct t_map::MapElement)*16*16*128];
};

class ParsedMapPaletteData
{
    public:
        bool valid;

        int cube_type;
        char name[DAT_NAME_MAX_LENGTH+1];

    void reset()
    {
        this->valid = false;
        this->cube_type = ERROR_CUBE;
        memset(this->name, 0, sizeof(this->name));
    }

    ParsedMapPaletteData()
    {
        this->reset();
    }
};

extern bool should_save_world;
extern bool map_save_memcpy_in_progress;

extern class BlockSerializer* block_serializer;

#if PTHREADS_ENABLED
void wait_for_threads();
bool save_map_iter(int max_ms);
#else
void wait_for_threads() {}
bool save_map_iter(int max_ms) { return true; }
#endif

bool save_map();
bool load_map();

void update_completed_map_save();

void init_map_serializer();
void teardown_map_serializer();

class BlockSerializer
{
    public:

        static const int prefix_length = sizeof(uint32_t);
        static const int version = GS_VERSION;
        static const int CHUNK_COUNT = MAP_CHUNK_XDIM*MAP_CHUNK_YDIM;

        char filename[GS_FN_MAX+1];
        size_t file_size;

        char* write_buffer;
        struct SerializedChunk* chunk;

        #if PTHREADS_ENABLED
        int version_array[CHUNK_COUNT];
        #else
        struct SerializedChunk chunks[CHUNK_COUNT];
        #endif

    BlockSerializer();
    ~BlockSerializer();

    static void push_int(char* buffer, int &index, uint32_t value)
    {
        *(reinterpret_cast<uint32_t*>(&buffer[index])) = value;
        index += sizeof(uint32_t);
    }

    static void pop_int(char* buffer, int &index, int &value)
    {
        value = *(reinterpret_cast<uint32_t*>(&buffer[index]));
        index += sizeof(uint32_t);
    }

    bool load(const char* filename);
    bool save(const char* filename);

    #if PTHREADS_ENABLED
    //this is called until map is done saving
    //will memcpy map and yield after ms milliseconds
    bool save_iter(int max_ms);
    #endif
};

}   // serializer
