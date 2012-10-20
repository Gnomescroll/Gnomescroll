#pragma once

#include <string.h>

#include <common/template/object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>

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

int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container);

bool parse_item_token(const char* key, const char* val, class ParsedItemData* data);

class Item::Item* create_item_from_data(class ParsedItemData* data, ItemLocationType location, int location_id);

}   // serializer
