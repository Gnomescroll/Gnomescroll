#pragma once


#include <common/files.hpp>

namespace t_map
{


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
    //int blockdata_size;
    struct SerializedChunk s[chunk_number];

    BlockSerializer()
    {
        memset(s, 0, chunk_number*sizeof(struct SerializedChunk));
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

        int ti1 = _GET_MS_TIME();

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

        int ti2 = _GET_MS_TIME();

        FILE* file = fopen(filename, "w");
        if(file == NULL)
        {
            printf("ERROR: cannot open map file %s \n", filename);
            return;
        }

        size_t ret = fwrite (buffer, sizeof(char), index, file);
        if (ferror(file))
            perror("Error with map save file: ");
        GS_ASSERT(ret == (size_t)index);
        fclose(file); /*done!*/ 

        int ti3 = _GET_MS_TIME();

        printf("BlockSerializer save: populate buffer took %i ms \n", ti2-ti1);
        printf("BlockSerializer save: writing file %s took %i ms \n", filename, ti3-ti2);

        free(buffer);
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
            memcpy((char*) &s[i], buffer+index, sizeof(struct SerializedChunk) );
            index += sizeof(struct SerializedChunk);
        }

        //serialize
        for(int i=0; i < chunk_number; i++)
        {
            class MAP_CHUNK* mp = main_map->chunk[i];
            GS_ASSERT(mp != NULL);
            if(mp == NULL) continue;
            memcpy(&mp->e, (void*) &s[i].data, 128*16*16*sizeof(struct MAP_ELEMENT));
        }

        int ti3 = _GET_MS_TIME();

        printf("BlockSerializer load: reading map file %s took %i ms \n", filename, ti2-ti1);
        printf("BlockSerializer load: loading map file %i ms \n", ti3-ti2);

        free(buffer);

        load_map_restore_containers();  //setup containers
    }
};

void save_map(char* filename)
{
    printf("Saving map...\n");
    create_path_to_file(filename);
    BlockSerializer* BS = new BlockSerializer;
    BS->save(filename);
    delete BS;
    printf("Map saved to %s\n", filename);
}


void load_map(char* filename)
{
    BlockSerializer* bs = new BlockSerializer;
    bs->load(filename);
    delete bs;  
}

const char default_map_file[] = "./world/map-" STR(DC_VERSION) ".map";

void save_map()
{
    save_map((char*)default_map_file );
}

void load_map()
{
    load_map((char*)default_map_file );
}

}   // t_map
