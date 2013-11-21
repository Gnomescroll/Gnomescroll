/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace serializer
{

const size_t BUF_SIZE = 0xffff-1;
extern char* _buffer;

extern ItemID* slot_checker;

// record keeping of container loading status
typedef enum
{
    CONTAINER_LOAD_UNUSED = 0,
    CONTAINER_LOAD_MAP,
    CONTAINER_LOAD_ITEMS
} ContainerLoadState;

extern ContainerLoadState* loaded_containers;

void init_state();
void teardown_state();

void clear_loaded_containers();

void clear_slot_checker();

}   // serializer
