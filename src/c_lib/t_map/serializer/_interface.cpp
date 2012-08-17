#include "_interface.hpp"

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
    
}   // t_map
