#pragma once


#include <common/files.hpp>

namespace t_map
{


struct SerializedChunk
{
    uint32_t xchunk;
    uint32_t ychunk;
    struct MAP_ELEMENT data[4*16*16*128];
};

class BlockSerializer
{
    public:

    static const int prefix_length = 4;
    static const int version = 0;
    static const int chunk_number = 16*16;
    //int blockdata_size;
    struct SerializedChunk s[16*16];

    BlockSerializer()
    {

    }

    void serialize()
    {

    }

    static void push_int(char* buffer, int &index, uint32_t value)
    {
        *((uint32_t*)(buffer+index)) = value;
        index += 4;
    }

    static void pop_int(char* buffer, int &index, int &value)
    {
        value = *((uint32_t*)(buffer+index));
        index += 4;
    }

    void save(const char* filename)
    {

        int ti1 = _GET_MS_TIME();

        //serialize
        for(int i=0; i < chunk_number; i++)
        {
            class MAP_CHUNK* mp = main_map->chunk[i];
            GS_ASSERT(mp != NULL);
            memcpy((void*) &s[i].data, &mp->e, 128*16*16*sizeof(struct MAP_ELEMENT));
        }
        //prepare buffer for saving

        int file_size = prefix_length + chunk_number*sizeof(struct SerializedChunk);

        char* buffer = (char*) malloc(file_size);
        int index = 0;

        push_int(buffer, index, version);

        for(int i=0; i<chunk_number; i++)
        {
            memcpy( buffer+index, (char*) &s[i], sizeof(SerializedChunk) );
            index += sizeof(SerializedChunk);
        }

        int ti2 = _GET_MS_TIME();

        FILE *file; 
        file = fopen(filename, "w+"); // apend file (add text to  a file or create a file if it does not exist. 
        //size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
        
        if(file == 0)
        {
        	printf("ERROR: cannot open map file %s \n", filename);
        	return;
        }

        int ret = fwrite (buffer, file_size, 1, file);
        GS_ASSERT(ret == 1);
        fclose(file); /*done!*/ 


        int ti3 = _GET_MS_TIME();

        printf("BlockSerializer save: populate buffer took %i ms \n", ti2-ti1);
        printf("BlockSerializer save: writing file %s took %i ms \n", filename, ti3-ti2);

        free(buffer);
    }


    void load(const char* filename)
    {
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
            memcpy((char*) &s[i], buffer+index, sizeof(SerializedChunk) );
            index += sizeof(SerializedChunk);
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
    }
};

void save_map()
{
    printf("Saving map...\n");
    
    const char fn[] = "./world/" STR(DC_VERSION) ".map";
    BlockSerializer* BS = new BlockSerializer;
    BS->save(fn);
    delete BS;
    printf("Map saved to %s\n", fn);
}

}   // t_map
