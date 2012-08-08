#include "_interface.cpp"

/*
class MAP_CHUNK
{
    public:

    int chunk_index;

    int xpos;
    int ypos;

    class CHUNK_ITEM_CONTAINER chunk_item_container;

    #if DC_CLIENT
    bool needs_update;
    #endif

    unsigned char top_block[MAP_CHUNK_XDIM*MAP_CHUNK_YDIM];

    struct MAP_ELEMENT e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT];

    MAP_CHUNK(int _xpos, int _ypos);

    inline int get_block(int x, int y, int z) __attribute__((always_inline)); //for VBO generator
    inline struct MAP_ELEMENT get_element(int x, int y, int z) __attribute__((always_inline)); //for VBO generator

    inline void set_element(int x, int y, int z, struct MAP_ELEMENT e) __attribute__((always_inline)); //for VBO generator

};

class Terrain_map
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    class MAP_CHUNK** chunk;
*/

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
		for(int i=0; i < 16*16; i++)
		{
			class MAP_CHUNK* mp = main_map->chunk[i];
			GS_ASSERT(mp != NULL);
			memcpy((void*) &s[i].data, &mp->e, 128*16*16*sizeof(struct MAP_ELEMENT));
		}
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
		
		int file_size = prefix_length + chunk_number*sizeof(SerializedChunk);

		char* buffer = new char[file_size]
		int index = 0;

		push_int(buffer, index, version);

		for(int i=0; i<chunk_number; i++)
		{
			memcpy(buffer+index, (void*) &s[i]), sizeof(sizeof(SerializedChunk)) );
			index += sizeof(SerializedChunk);
		}

		FILE *file; 
		file = fopen(filename, "w+"); /* apend file (add text to  a file or create a file if it does not exist.*/ 
		//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
		int ret = fwrite (buffer, file_size, 1, file);
		GS_ASSERT(ret == 1);
		fclose(file); /*done!*/ 
/*
		while(windex < file_size)
		{
			windex += fwrite(file, )
		}
*/

	}
};

	
}