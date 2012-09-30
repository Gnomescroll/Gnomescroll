#include "items.hpp"

#include <stdio.h>
#include <stdint.h>

#include <common/files.hpp>
#include <item/_interface.hpp>

namespace serializer
{

// TODO -- record all errors to a file. email? save the data too?
// TODO -- memcpy all data to a thread

uint32_t item_global_id = 0;

#define ITEM_ID_TOKEN         "GID"
#define ITEM_LOC_TOKEN        "LOC"
#define ITEM_TYPE_TOKEN       "TYP"
#define ITEM_DURABILITY_TOKEN "DUR"
#define ITEM_STACK_SIZE_TOKEN "STK"

static const char* get_item_filename(uint32_t global_id)
{
    // make filename
    static const int fnlen = sizeof(item_filename_fmt) + 10 - 4;
    static char fnbuf[fnlen+1];
    int ret = snprintf(fnbuf, fnlen+1, item_filename_fmt, global_id);
    GS_ASSERT(ret < fnlen+1);   // did we fit the filename into the buffer
    if (ret >= fnlen + 1) return NULL;
    return fnbuf;
}

SerializerError save_item(ItemID item_id)
{
    // get item
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return SE_SAVE_ITEM_ITEM_NOT_FOUND;
    GS_ASSERT(item_id == item->id);
    if (item_id != item->id) return SE_SAVE_ITEM_ITEM_ID_MISMATCH;

    // TODO -- assert valid values on item
    
    // make filename
    const char* fn = get_item_filename(item->global_id);
    GS_ASSERT(fn != NULL);
    if (fn == NULL) return SE_SAVE_ITEM_FILENAME_LONG;

    // create file
    FILE* fp = fopen(fn, "w");
    GS_ASSERT(fp != NULL);
    if (fp == NULL) return SE_SAVE_ITEM_NULL_FILE;

    // write data

    // ID
    fprintf(fp, ITEM_ID_TOKEN " %010d\n", item->global_id);
    // Location
    fprintf(fp, ITEM_LOC_TOKEN " %d %d %d\n", item->location, item->location_id, item->container_slot);
    // Attributes
    //item->serialize(fp);

    // cleanup
    fclose(fp);

    return SE_NONE;
}

SerializerError load_item(uint32_t global_id)
{
    const char* fn = get_item_filename(global_id);
    GS_ASSERT(fn != NULL);
    if (fn == NULL) return SE_SAVE_ITEM_FILENAME_LONG;

    int n = -1;
    char* data = read_file_to_buffer(fn, &n);
    GS_ASSERT(data != NULL);
    GS_ASSERT(n > 0);
    if (data != NULL || n <= 0) return SE_LOAD_ITEM_DATA_NOT_FOUND;

    // split into lines
    // parse the file

    // create the item

    //Item::Item* item = Item::create_item(item_name);
    //GS_ASSERT(item != NULL);
    //if (item == NULL) return SE_LOAD_ITEM_CREATE_ITEM_FAILED;
    

    free(data);

    return SE_NONE;
}

void save_items()
{
    using Item::item_list;
    GS_ASSERT(item_list != NULL);
    if (item_list == NULL) return;

    // bulk memcpy for threaded write first
    
    for (int i=0; i<item_list->n_max; i++)
        if (item_list->a[i] != NULL)
            save_item((ItemID)i);
}

SerializerError load_item_global_id()
{
    GS_ASSERT(item_global_id == 0);

    if (!file_exists(item_global_id_filename)) return SE_NONE;
    
    int n = -1;
    char* buf = read_file_to_buffer(item_global_id_filename, &n);
    GS_ASSERT(buf != NULL);
    GS_ASSERT(n > 0);

    // TODO -- this is an error if the global id is not found but there are items floating around in the folder
    // item id rollover is not a problem -- we'll have a separate item server by then that will handle it
    
    if (buf == NULL || n <= 0)
    {
        item_global_id = 0;
        return SE_LOAD_ITEM_GLOBAL_ID_FILE_ERROR;
    }

    item_global_id = (int32_t)atoi(buf);
    free(buf);

    GS_ASSERT(item_global_id > 0);
    if (item_global_id <= 0) return SE_LOAD_ITEM_GLOBAL_ID_INVALID;

    return SE_NONE;
}

SerializerError write_item_global_id()
{
    if (item_global_id == 0) return SE_NONE;

    FILE* fp = fopen(item_global_id_filename, "w");
    GS_ASSERT(fp != NULL);
    if (fp == NULL) return SE_WRITE_ITEM_GLOBAL_ID_FILE_ERROR;

    fprintf(fp, "%d", item_global_id);

    fclose(fp);
    
    return SE_NONE;
}

uint32_t get_new_item_global_id()
{
    GS_ASSERT(item_global_id != UINT32_MAX);
    // TODO -- report error
    if (item_global_id == UINT32_MAX) return 0;
    return ++item_global_id;
}

}   // serializer
