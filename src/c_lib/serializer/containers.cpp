#include "containers.hpp"

#include <stdio.h>

#include <item/container/container.hpp>
#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <serializer/items.hpp>
#include <serializer/_state.hpp>

namespace serializer
{

const char* write_container_string(const class ItemContainer::ItemContainerInterface* container, int container_entry, size_t* wrote)
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
        container_entry, container_name, container->slot_count, b[0], b[1], b[2]);
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

    // write separator between container entries
    could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, CONTAINER_SEPARATOR "\n");
    GS_ASSERT(could_write > 0 && (size_t)could_write < BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf) return NULL;
    ibuf += (size_t)could_write;

    _buffer[ibuf++] = '\n';
    if (ibuf >= BUF_SIZE) return NULL;

    _buffer[ibuf] = '\0';

    *wrote = ibuf;
    return _buffer;
}

static bool save_container(FILE* f, const class ItemContainer::ItemContainerInterface* container, int container_entry)
{
    size_t container_string_length = 0;
    const char* container_string = write_container_string(container, container_entry, &container_string_length);
    
    GS_ASSERT(container_string != NULL);
    if (container_string == NULL) return false;
    GS_ASSERT(container_string_length > 0);
    if (container_string_length <= 0) return false;
    
    size_t wrote = fwrite(container_string, sizeof(char), container_string_length, f);
    
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
        if (item_container_list->a[i] != NULL && !item_container_list->a[i]->attached_to_agent)
        {
            bool success = save_container(f, item_container_list->a[i], i+1);
            GS_ASSERT(success); // TODO -- log error
        }

    int err = fclose(f);
    GS_ASSERT(!err);
    if (err) return;   // TODO -- log error

    // move current file to .bak
    if (file_exists(container_filename))
    {
        err = rename(container_filename, container_filename_backup);
        GS_ASSERT(!err);
        if (err) return;   // TODO -- log error
    }
    
    // move tmp file to current
    err = rename(container_filename_tmp, container_filename);
    GS_ASSERT(!err);
}

class ParsedContainerFileData
{
    public:

        int version;
        int container_count;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->version = 0;
        this->container_count = 0;
    }

    ParsedContainerFileData()
    {
        this->reset();
    }
};

class ParsedContainerData
{
    public:

        int container_id;
        char name[CONTAINER_NAME_MAX_LENGTH+1];
        int item_count;
        struct
        {
            int x,y,z;
        } position;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->container_id = -1;
        this->name[0] = '\0';
        this->item_count = 0;
        this->position.x = -1;
        this->position.y = -1;
        this->position.z = -1;
    }

    ParsedContainerData()
    {
        this->reset();
    }
};


// WARNING: modifies char* str
static void parse_container_file_header(char* str, size_t length, class ParsedContainerFileData* data)
{
    data->valid = false;
    // TODO
    data->valid = true;
}

// WARNING: modifies char* str
static void parse_container_header(char* str, size_t length, class ParsedContainerData* data)
{
    data->valid = false;
    // TODO
    data->valid = true;
}

static bool process_container_blob(const char* str, size_t filesize)
{
    // allocate scratch buffer
    static const size_t LONGEST_LINE_A = GS_MAX(CONTAINER_LINE_LENGTH, CONTAINER_FILE_LINE_LENGTH);
    static const size_t LONGEST_LINE_B = GS_MAX(ITEM_LINE_LENGTH, CONTAINER_SEPARATOR_LENGTH);
    static const size_t LONGEST_LINE = GS_MAX(LONGEST_LINE_A, LONGEST_LINE_B);
    static char buf[LONGEST_LINE+1];
    if (strnlen(str, LONGEST_LINE) < LONGEST_LINE) return false;  // TODO -- error handling
    buf[LONGEST_LINE] = '\0';

    // copy file header line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i] = '\0';
    GS_ASSERT(c == '\n');
    if (c != '\n') return false;
    
    // read file header
    class ParsedContainerFileData container_file_data;
    parse_container_file_header(buf, i, &container_file_data);
    GS_ASSERT(container_file_data.valid);
    if (!container_file_data.valid) return false;
    i++;

    class ParsedContainerData container_data;
    class ParsedItemData item_data;
    for (int m=0; m<container_file_data.container_count; m++)
    {
        GS_ASSERT(c != '\0');
        if (c == '\0') return false;
        
        // copy ctr header line to buffer
        size_t k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
        buf[k] = '\0';
        GS_ASSERT(c == '\n');
        if (c != '\n') return false;

        // parse ctr header
        container_data.reset();
        parse_container_header(buf, k, &container_data);
        GS_ASSERT(container_data.valid);
        if (!container_data.valid) return false;
        GS_ASSERT(container_data.container_id == m+1);  // not fatal, but indicates a problem with the code

        // get container type
        // TODO -- apply renaming scheme
        ItemContainerType container_type = ItemContainer::get_type(container_data.name);
        GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
        if (container_type == CONTAINER_TYPE_NONE) return false;

        // create container
        class ItemContainer::ItemContainerInterface* container = ItemContainer::create_container(container_type);
        GS_ASSERT(container != NULL);
        if (container == NULL) return false;
        ItemContainer::init_container(container);

        // check items will fit
        GS_ASSERT(container_data.item_count <= container->slot_max);
        if (container_data.item_count > container->slot_max) return false;

        // parse contents
        for (int n=0; n<container_data.item_count; n++)
        {
            // copy item line to buffer
            k = 0;
            while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
                buf[k++] = c;
            buf[k] = '\0';
            GS_ASSERT(c == '\n');
            if (c != '\n') return false;
            
            item_data.reset();
            parse_item_string(buf, k, &item_data);
            GS_ASSERT(item_data.valid);
            if (!item_data.valid) return false;
            if (!create_item_from_data(&item_data, IL_CONTAINER, container->id)) return false;
        }

        // check separator
        k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = '\0';
        buf[k] = '\0';
        GS_ASSERT(c == '\n');
        if (c != '\n') return false;

        int sepmatch = strcmp(buf, CONTAINER_SEPARATOR);
        GS_ASSERT(sepmatch == 0);
        if (sepmatch != 0) return false;
    }

    // make sure that we reached the end of the file. otherwise the reported container count is too low
    GS_ASSERT(str[i] == '\0');
    if (str[i] != '\0') return false;

    return true;
}

static bool load_container_file(const char* filename)
{
    printf("Loading container file %s\n", filename);
    
    size_t filesize = 0;
    char* buffer = read_file_to_buffer(filename, &filesize);
    GS_ASSERT(buffer != NULL);
    if (buffer == NULL) return false;

    return process_container_blob(buffer, filesize);
}

bool load_containers()
{
    if (file_exists(container_filename) && fsize(container_filename) > 0)
        return load_container_file(container_filename);
    else
    if (file_exists(container_filename_backup) && fsize(container_filename_backup) > 0)
        return load_container_file(container_filename_backup);
    else
        printf("WARNING: No container file found\n");
    return true;
}

}   // serializer
