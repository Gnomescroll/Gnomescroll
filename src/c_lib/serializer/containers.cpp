#include "containers.hpp"

#include <stdio.h>

#include <item/container/container.hpp>
#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <serializer/items.hpp>
#include <serializer/_state.hpp>

namespace serializer
{

const char* write_container_string(class ItemContainer::ItemContainerInterface* container, size_t* wrote)
{
    *wrote = 0;
    size_t ibuf = 0;

    // get container data
    const char* container_name = ItemContainer::get_container_name(container->type);
    GS_ASSERT(container_name != NULL);
    if (container_name == NULL) return NULL;

    int b[3];
    bool found = t_map::get_container_location(container->id, b);
    GS_ASSERT(found);
    if (!found) return NULL;
    bool fits = (b[0] >= 0 && b[0] < XMAX
               && b[1] >= 0 && b[1] < YMAX
               && b[2] >= 0 && b[2] < ZMAX);
    GS_ASSERT(fits);
    if (!fits) return NULL;

    // write header
    int could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, CONTAINER_HEADER_FMT,
        container_name, container->slot_count, b[0], b[1], b[2]);
    GS_ASSERT(could_write > 0 && (size_t)could_write < BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf) return NULL;
    ibuf += (size_t)could_write;

    _buffer[ibuf++] = '\n';
    if (ibuf >= BUF_SIZE) return NULL;

    // write contents
    if (container->slot_count > 0)
    {   // we must check the slot count, because write_container_contents_string returns 0 on error or if the container is empty
        size_t wrote = write_container_contents_string(&_buffer[ibuf], BUF_SIZE - ibuf, container);
        if (wrote <= 0) return NULL;  // error
        ibuf += wrote;
    }

    _buffer[ibuf] = '\0';

    *wrote = ibuf;
    return _buffer;
}

static bool save_container(FILE* f, class ItemContainer::ItemContainerInterface* container)
{
    size_t container_string_length = 0;
    const char* container_string = write_container_string(container, &container_string_length);
    
    GS_ASSERT(container_string != NULL);
    if (container_string == NULL) return false;
    GS_ASSERT(container_string_length > 0);
    if (container_string_length <= 0) return false;
    
    size_t wrote = fwrite(container_string, container_string_length, sizeof(char), f);
    
    GS_ASSERT(wrote == container_string_length);
    return (wrote == container_string_length);
}

void save_containers()
{
    using ItemContainer::item_container_list;
    GS_ASSERT(item_container_list != NULL);
    if (item_container_list == NULL) return;    // TODO -- log error

    FILE* f = fopen(container_filename_tmp, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL) return;  // TODO -- log error

    for (int i=0; i<item_container_list->n_max; i++)
        if (item_container_list->a[i] != NULL)
        {
            bool success = save_container(f, item_container_list->a[i]);
            GS_ASSERT(success); // TODO -- log error
        }

    int ret = fclose(f);
    if (!ret) return;   // TODO -- log error
    GS_ASSERT(ret);

    // move current file to .bak
    ret = rename(container_filename, container_filename_backup);
    GS_ASSERT(ret);
    if (!ret) return;   // TODO -- log error
    
    // move tmp file to current
    ret = rename(container_filename_tmp, container_filename);
    GS_ASSERT(ret);
}

}   // serializer
