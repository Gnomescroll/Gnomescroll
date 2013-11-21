/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_state.hpp"

namespace serializer
{

char* _buffer = NULL;
ContainerLoadState* loaded_containers;

ItemID* slot_checker = NULL;

void init_state()
{
    GS_ASSERT(_buffer == NULL);
    _buffer = (char*)calloc(BUF_SIZE+1, sizeof(char));

    GS_ASSERT(loaded_containers == NULL);
    loaded_containers = (ContainerLoadState*)calloc(MAX_CONTAINERS, sizeof(ContainerLoadState));
    for (int i=0; i<MAX_CONTAINERS; loaded_containers[i++] = CONTAINER_LOAD_UNUSED);

    GS_ASSERT(slot_checker == NULL);
    slot_checker = (ItemID*)malloc(MAX_CONTAINER_SIZE * sizeof(ItemID));
    clear_slot_checker();
}

void teardown_state()
{
    free(_buffer);
    free(loaded_containers);
    free(slot_checker);
}

void clear_loaded_containers()
{   // we dont need this memory after startup
    if (loaded_containers != NULL)
    {
        free(loaded_containers);
        loaded_containers = NULL;
    }
}

void clear_slot_checker()
{
    for (size_t i=0; i<MAX_CONTAINER_SIZE; slot_checker[i++] = NULL_ITEM);
}

}   // serializer
