#include "items.hpp"

#include <item/item.hpp>
#include <item/common/constants.hpp>
#include <serializer/redis.hpp>
#include <serializer/_state.hpp>
#include <serializer/uuid.hpp>

namespace serializer
{

class ParsedItemData
{
    public:
        uuid_t uuid;
        char name[ITEM_NAME_MAX_LENGTH+1];
        unsigned int durability;
        unsigned int stack_size;
        char location_name[LOCATION_NAME_MAX_LENGTH+1];
        unsigned int location_id;
        unsigned int container_slot;
        bool valid;

    void reset()
    {
        this->valid = false;
        uuid_clear(this->uuid);
        memset(this->name, 0, sizeof(this->name));
        this->durability = NULL_DURABILITY;
        this->stack_size = NULL_STACK_SIZE;
        memset(this->location_name, 0, sizeof(this->location_name));
        this->location_id = NULL_LOCATION;
        this->container_slot = NULL_SLOT;
    }

    ParsedItemData()
    {
        this->reset();
    }
};

// WARNING -- modifies char* str
void parse_item_string(char* str, const size_t length, class ParsedItemData* data)
{
    data->valid = false;

    int i = (int)length;
    size_t token_length = 0;
    while (i >= 0)
    {
        char c = str[i];
        if (c == ' ') str[i] = '\0';    // convert all spaces to NUL so that padded strings get shortened
        if (i && c != PROPERTY_DELIMITER[0])
        {
            i--;
            token_length++;
            continue;
        }
        GS_ASSERT(token_length >= TAG_LENGTH + TAG_DELIMITER_LENGTH);
        if (token_length < TAG_LENGTH + TAG_DELIMITER_LENGTH) return;
        
        char* key = NULL;
        if (i)
        {
            str[i] = '\0';
            key = &str[i+1];
        }
        else
            key = &str[i];
            
        char* val = &key[TAG_LENGTH+TAG_DELIMITER_LENGTH];

        bool err = false;
        if (strncmp(UUID_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            int ret = uuid_parse(val, data->uuid);
            GS_ASSERT(!ret);
            if (ret) return;
            ret = uuid_is_null(data->uuid);
            GS_ASSERT(!ret);
            if (ret) return;
        }
        else
        if (strncmp(NAME_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            GS_ASSERT(Item::is_valid_item_name(val));
            if (!Item::is_valid_item_name(val)) return;
            strncpy(data->name, val, ITEM_NAME_MAX_LENGTH);
            data->location_name[ITEM_NAME_MAX_LENGTH] = '\0';
        }
        else
        if (strncmp(DURABILITY_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long durability = parse_int(val, err);
            GS_ASSERT(!err && durability > 0 && durability <= MAX_DURABILITY);
            if (err || durability <= 0 || durability > MAX_DURABILITY) return;
            data->durability = (int)durability;
        }
        else
        if (strncmp(STACK_SIZE_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long stack_size = parse_int(val, err);
            GS_ASSERT(!err && stack_size > 0 && stack_size <= MAX_STACK_SIZE);
            if (err || stack_size <= 0 || stack_size > MAX_STACK_SIZE) return;
            data->stack_size = (int)stack_size;
        }
        else
        if (strncmp(CONTAINER_SLOT_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long container_slot = parse_int(val, err);
            GS_ASSERT(!err && ((container_slot >= 0 && container_slot <= MAX_CONTAINER_SIZE) || container_slot == NULL_SLOT));
            if (err || ((container_slot < 0 || container_slot > MAX_CONTAINER_SIZE) && container_slot != NULL_SLOT)) return;
            data->container_slot = container_slot;
        }
        else
        {   // unrecognized field
            GS_ASSERT(false);
            data->valid = false;
            return;
        }

        i--;
        token_length = 0;   // reset
    }

    data->valid = true;
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
        uuid_buf,
        item_name,
        item->durability,
        item->stack_size,
        item->container_slot);

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

}   // serializer
