#include "_state.hpp"

namespace serializer
{

char* _buffer = NULL;
ContainerLoadState* loaded_containers;

void init_state()
{
    GS_ASSERT(_buffer == NULL);
    _buffer = (char*)calloc(BUF_SIZE+1, sizeof(char));

    GS_ASSERT(loaded_containers == NULL);
    loaded_containers = (ContainerLoadState*)calloc(MAX_CONTAINERS, sizeof(ContainerLoadState));
    for (int i=0; i<MAX_CONTAINERS; loaded_containers[i++] = CONTAINER_LOAD_UNUSED);
}

void teardown_state()
{
    if (_buffer != NULL) free(_buffer);
    if (loaded_containers != NULL) free(loaded_containers);
}

void clear_loaded_containers()
{   // we dont need this memory after startup
    if (loaded_containers != NULL)
    {
        free(loaded_containers);
        loaded_containers = NULL;
    }
}

}   // serializer
