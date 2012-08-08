#pragma once



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

	static void push_int(char* buffer, int &_index, uint32_t value)
	{
		*((uint32_t*)(buffer+_index)) = value;
		_index += 4;
	}

	static void pop_int(char* buffer, int &_index, int &value)
	{
		value = *((uint32_t*)(buffer+_index));
		_index += 4;
	}

	void save(char* filename)
	{
		
		int file_size = prefix_length + chunk_number*sizeof(struct SerializedChunk);

		char* buffer = new char[file_size];
		int _index = 0;

		push_int(buffer, _index, version);

		for(int i=0; i<chunk_number; i++)
		{
			memcpy( buffer+_index, (char*) &s[i], sizeof(SerializedChunk) );
			_index += sizeof(SerializedChunk);
		}

		FILE *file; 
		file = fopen(filename, "w+"); /* apend file (add text to  a file or create a file if it does not exist.*/ 
		//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
		int ret = fwrite (buffer, file_size, 1, file);
		GS_ASSERT(ret == 1);
		fclose(file); /*done!*/ 
	/*
		while(w_index < file_size)
		{
			w_index += fwrite(file, )
		}
	*/
	}

};

}
