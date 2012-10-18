#include "items.hpp"

#include <item/item.hpp>
#include <item/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/common/constants.hpp>
#include <serializer/redis.hpp>
#include <serializer/_state.hpp>
#include <serializer/uuid.hpp>

namespace serializer
{

// WARNING -- modifies char* str
bool parse_item_token(const char* key, const char* val, class ParsedItemData* data)
{
    bool err = false;
    if (strcmp(UUID_TAG, key) == 0)
    {
        int ret = uuid_parse(val, data->uuid);
        GS_ASSERT(!ret);
        if (ret) return false;
        ret = uuid_is_null(data->uuid);
        GS_ASSERT(!ret);
        if (ret) return false;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = Item::is_valid_item_name(val);
        GS_ASSERT(valid_name);
        if (!valid_name) return false;
        strncpy(data->name, val, ITEM_NAME_MAX_LENGTH);
        data->location_name[ITEM_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(DURABILITY_TAG, key) == 0)
    {
        long long durability = parse_int(val, err);
        GS_ASSERT(!err && durability > 0 && durability <= MAX_DURABILITY);
        if (err || durability <= 0 || durability > MAX_DURABILITY) return false;
        data->durability = (int)durability;
    }
    else
    if (strcmp(STACK_SIZE_TAG, key) == 0)
    {
        long long stack_size = parse_int(val, err);
        GS_ASSERT(!err && stack_size > 0 && stack_size <= MAX_STACK_SIZE);
        if (err || stack_size <= 0 || stack_size > MAX_STACK_SIZE) return false;
        data->stack_size = (int)stack_size;
    }
    else
    if (strcmp(CONTAINER_SLOT_TAG, key) == 0)
    {
        long long container_slot = parse_int(val, err);
        GS_ASSERT(!err && ((container_slot >= 0 && container_slot <= MAX_CONTAINER_SIZE) || container_slot == NULL_SLOT));
        if (err || ((container_slot < 0 || container_slot > MAX_CONTAINER_SIZE) && container_slot != NULL_SLOT)) return false;
        data->container_slot = container_slot;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        data->valid = false;
        return false;
    }
    return true;
}

static inline bool item_valid(class Item::Item* item)
{
    // the item valid check occurs every frame and covers most item state
    // it doesnt check uuids, as they are serializer specific and expensive to check, and won't change
    if (!item->valid) return false;
    if (item->location == IL_PARTICLE) return false;    // We don't support item particle saving yet
    // recover from missing UUID error. this is a programming error, but is not fatal.
    bool uuid_valid = (!uuid_is_null(item->uuid));
    GS_ASSERT(uuid_valid);
    if (!uuid_valid) uuid_generate(item->uuid);
    return true;
}

size_t write_item_string(char* buf, size_t buffer_size, ItemID item_id)
{
    // TODO -- check/acquire global id
    // just get a global id block from redis periodically

    // TODO -- validate item

    class Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return 0;

    bool valid = item_valid(item);
    GS_ASSERT(valid);
    if (!valid) return 0;

    const char* item_name = Item::get_item_name(item->type);
    GS_ASSERT(item_name != NULL);
    if (item_name == NULL) return 0;

    static char uuid_buf[UUID_STRING_LENGTH+1];
    size_t wrote = write_uuid(uuid_buf, UUID_STRING_LENGTH+1, item->uuid);
    if (wrote != UUID_STRING_LENGTH) return 0;

    int could_write = snprintf(buf, buffer_size, ITEM_FMT,
        item_name,
        item->durability,
        item->stack_size,
        item->container_slot,
        uuid_buf);

    if (could_write < 0) return could_write;
    if ((size_t)could_write >= buffer_size)
    {
        buf[0] = '\0';
        return 0;
    }
    
    buf[could_write++] = '\n';

    return (size_t)could_write;
}

// returns 0 on failure. can also return 0 if there are no container contents, so check the container first
int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container)
{
    size_t ibuf = 0;
    for (int i=0; i<container->slot_max; i++)
        if (container->slot[i] != NULL_ITEM)
        {
            int could_write = write_item_string(&buf[ibuf], buffer_size - ibuf, container->slot[i]);
            if (could_write < 0) return could_write;                    // c lib printf returned error
            if ((size_t)could_write >= buffer_size - ibuf) return 0;    // ran out of buffer space
            ibuf += (size_t)could_write;
        }
    buf[ibuf] = '\0';
    return (int)ibuf;
}

class Item::Item* create_item_from_data(class ParsedItemData* data, ItemLocationType location, int location_id)
{
    // TODO -- make sure item can be placed in location (is not occupied etc)
    
    // TODO - Apply renaming scheme to get item type
    int item_type = Item::get_item_type(data->name);
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return NULL;  // TODO -- log error

    // create item
    class Item::Item* item = Item::create_item_for_loading();
    GS_ASSERT(item != NULL);
    if (item == NULL) return NULL; // TODO -- log error. RESERVE ITEM SPACE FOR LOADING

    uuid_copy(item->uuid, data->uuid);
    item->type = item_type;
    item->durability = data->durability;
    item->stack_size = data->stack_size;
    item->container_slot = data->container_slot;
    item->location = location;
    item->location_id = location_id;

    item->init_for_loading();        

    return item;
}

}   // serializer
