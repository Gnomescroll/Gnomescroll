#include "containers.hpp"

#include <stdio.h>

#include <item/container/container.hpp>
#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <serializer/items.hpp>
#include <serializer/_state.hpp>
#include <serializer/parse.hpp>

namespace serializer
{

/* Parse data structs */

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
        char name[DAT_NAME_MAX_LENGTH+1];
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

/* Loading */

static bool parse_container_file_header_token(const char* key, const char* val, class ParsedContainerFileData* data)
{
    bool err = false;
    if (strcmp(VERSION_TAG, key) == 0)
    {
        long long version = parse_int(val, err);
        GS_ASSERT(!err && version > 0 && version <= GS_VERSION);
        if (err || version <= 0 || version > GS_VERSION) return false;
        data->version = version;
    }
    else
    if (strcmp(CONTAINER_COUNT_TAG, key) == 0)
    {
        long long container_count = parse_int(val, err);
        GS_ASSERT(!err && container_count >= 0 && container_count <= MAX_CONTAINERS);
        if (err || container_count < 0 || container_count > MAX_CONTAINERS) return false;
        data->container_count = container_count;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool parse_container_token(const char* key, const char* val, class ParsedContainerData* data)
{
    bool err = false;
    if (strcmp(CONTAINER_ID_TAG, key) == 0)
    {
        long long container_id = parse_int(val, err);
        GS_ASSERT(!err && container_id >= 0 && container_id < MAX_CONTAINERS);
        if (err || container_id < 0 || container_id >= MAX_CONTAINERS) return false;
        data->container_id = container_id;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = ItemContainer::is_valid_container_name(val);
        GS_ASSERT(valid_name);
        if (!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(CONTAINER_ITEM_COUNT_TAG, key) == 0)
    {
        long long item_count = parse_int(val, err);
        GS_ASSERT(!err && item_count >= 0 && item_count <= MAX_CONTAINER_SIZE);
        if (err || item_count < 0 || item_count > MAX_CONTAINER_SIZE) return false;
        data->item_count = item_count;
    }
    else
    if (strcmp(MAP_POSITION_TAG, key) == 0)
    {
        static char buf[MAP_POSITION_LENGTH+1] = {'\0'};
        strncpy(buf, val, MAP_POSITION_LENGTH);
        buf[MAP_POSITION_LENGTH] = '\0';
        int pts = 1;
        char d;
        int j = 0;
        int cmp_len = 0;
        while ((d = buf[j]) != '\0')
        {
            if (d == MAP_POSITION_COMPONENT_DELIMITER[0])
            {
                GS_ASSERT(cmp_len == MAP_POSITION_COMPONENT_LENGTH);
                if (cmp_len != MAP_POSITION_COMPONENT_LENGTH) return false;
                cmp_len = 0;
                pts++;
                buf[j] = '\0';
            }
            else
                cmp_len++;
            j++;
        }
        GS_ASSERT(cmp_len == MAP_POSITION_COMPONENT_LENGTH);
        if (cmp_len != MAP_POSITION_COMPONENT_LENGTH) return false;
        GS_ASSERT(pts == 3);
        if (pts != 3) return false;
        
        int base_offset = MAP_POSITION_COMPONENT_LENGTH + MAP_POSITION_COMPONENT_DELIMITER_LENGTH;
        long long x = parse_int(&buf[0 * base_offset], err);
        GS_ASSERT(!err && x >= 0 && x < XMAX);
        if (err || x < 0 || x >= XMAX) return false;
        long long y = parse_int(&buf[1 * base_offset], err);
        GS_ASSERT(!err && y >= 0 && y < YMAX);
        if (err || y < 0 || y >= YMAX) return false;
        long long z = parse_int(&buf[2 * base_offset], err);
        GS_ASSERT(!err && z >= 0 && z < ZMAX);
        if (err || z < 0 || z >= ZMAX) return false;

        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool process_container_blob(const char* str, size_t filesize)
{
    // allocate scratch buffer
    static const size_t LONGEST_LINE_A = GS_MAX(CONTAINER_LINE_LENGTH, CONTAINER_FILE_LINE_LENGTH);
    static const size_t LONGEST_LINE_B = GS_MAX(ITEM_LINE_LENGTH, CONTAINER_SEPARATOR_LENGTH);
    static const size_t LONGEST_LINE = GS_MAX(LONGEST_LINE_A, LONGEST_LINE_B);

    char buf[LONGEST_LINE+1] = {'\0'};

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
    parse_line<class ParsedContainerFileData>(&parse_container_file_header_token, buf, i, &container_file_data);
    GS_ASSERT(container_file_data.valid);
    if (!container_file_data.valid) return false;
    i++;

    class ParsedContainerData container_data;
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
        parse_line<class ParsedContainerData>(&parse_container_token, buf, k, &container_data);
        GS_ASSERT(container_data.valid);
        if (!container_data.valid) return false;
        GS_ASSERT(container_data.container_id == m+1);  // not fatal, but indicates a problem with container serializer

        // get container by map position
        int container_id = t_map::get_block_item_container(container_data.position.x,
            container_data.position.y, container_data.position.z);
        GS_ASSERT(container_id != NULL_CONTAINER);
        if (container_id == NULL_CONTAINER) return false;

        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return false;

        // check container type
        // TODO -- apply renaming scheme
        const char* actual_name = ItemContainer::get_compatible_name(container_data.name);
        GS_ASSERT(actual_name != NULL);
        if (actual_name == NULL) return false;

        ItemContainerType container_type = ItemContainer::get_type(actual_name);
        GS_ASSERT(container_type == container->type);
        if (container_type != container->type) return false;

        // check items will fit
        GS_ASSERT(container_data.item_count <= container->slot_max);
        if (container_data.item_count > container->slot_max) return false;

        clear_slot_checker();
        
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
            
            class ParsedItemData* item_data = create_item_data();
            parse_line<class ParsedItemData>(&parse_item_token, buf, k, item_data);
            GS_ASSERT(item_data->valid);
            if (!item_data->valid) return false;

            item_data->item_location = IL_CONTAINER;

            int item_type = Item::get_item_type(item_data->name);
            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            if (item_type == NULL_ITEM_TYPE) return false;

            item_data->item_type = item_type;

            GS_ASSERT(slot_checker[item_data->container_slot] == NULL_ITEM);
            if (slot_checker[item_data->container_slot] != NULL_ITEM) return false;
            slot_checker[item_data->container_slot] = item_data->id;
        }

        if (!create_container_items_from_data(container->id)) return false;

        // mark container with next phase of loaded status 
        loaded_containers[container->id] = CONTAINER_LOAD_ITEMS;

        // check separator
        k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
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

    bool success = process_container_blob(buffer, filesize);
    if (!success) return false;

    // check that all containers loaded from map were also found in the flat file
    for (int i=0; i<MAX_CONTAINERS; i++)
    {
        ContainerLoadState state = loaded_containers[i];
        GS_ASSERT(state == CONTAINER_LOAD_UNUSED || state == CONTAINER_LOAD_ITEMS);
        // TODO -- collect all unloaded containers and log their positions/block type
        if (state != CONTAINER_LOAD_UNUSED && state != CONTAINER_LOAD_ITEMS) return false;
    }

    clear_loaded_containers();

    return true;
}

/* Saving */

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
        container_name, container->slot_count, b[0], b[1], b[2], container_entry);
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


/* Public Interface */

void save_containers()
{
    using ItemContainer::item_container_list;
    GS_ASSERT(item_container_list != NULL);
    if (item_container_list == NULL) return;    // TODO -- log error

    FILE* f = fopen(container_filename_tmp, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL) return;  // TODO -- log error

    // write a temporary file header. the container count is wrong, so we have to rewrite it later 
    int ret = fprintf(f, CONTAINER_FILE_HEADER_FMT, GS_VERSION, item_container_list->n_max);
    GS_ASSERT(ret > 0);
    if (ret <= 0) return;   // TODO -- log error
    ret = fprintf(f, "\n");
    GS_ASSERT(ret > 0);
    if (ret <= 0) return;   // TODO -- log error

    // write the container contents
    int ct = 0;
    for (unsigned i=0; i<item_container_list->n_max; i++)
        if (item_container_list->a[i] != NULL && !item_container_list->a[i]->attached_to_agent)
        {
            bool success = save_container(f, item_container_list->a[i], ct+1);
            GS_ASSERT(success); // TODO -- log error
            if (success) ct++;
        }

    // rewind to beginning of file
    rewind(f);

    // write the actual file header
    ret = fprintf(f, CONTAINER_FILE_HEADER_FMT, GS_VERSION, ct);
    GS_ASSERT(ret > 0);
    if (ret <= 0) return;   // TODO -- log error
    ret = fprintf(f, "\n");
    GS_ASSERT(ret > 0);
    if (ret <= 0) return;   // TODO -- log error

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
