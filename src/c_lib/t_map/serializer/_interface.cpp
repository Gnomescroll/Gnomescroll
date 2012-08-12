#include "_interface.hpp"

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

BlockSerializer* block_serializer = NULL;

void init_map_serializer()
{
    GS_ASSERT(block_serializer == NULL);
    block_serializer = new BlockSerializer;
}

void teardown_map_serializer()
{
    if (block_serializer != NULL) delete block_serializer;
}

    
}
