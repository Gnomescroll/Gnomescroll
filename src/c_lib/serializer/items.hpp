#pragma once

#include <string.h>

#include <common/template/object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>
#include <item/item.hpp>

namespace serializer
{

class ParsedItemData
{
    public:
        ItemID id;

        uuid_t uuid;
        char name[DAT_NAME_MAX_LENGTH+1];
        unsigned int durability;
        unsigned int stack_size;
        char location_name[LOCATION_NAME_MAX_LENGTH+1];
        int location_id;
        unsigned int container_slot;
        bool valid;

        // additional item data attached after processing, but before item creation
        ItemType item_type;
        ItemLocationType item_location;
        ItemContainerType item_container_type;

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

        this->item_type = NULL_ITEM_TYPE;
        this->item_location = IL_NOWHERE;
        this->item_container_type = NULL_CONTAINER_TYPE;
    }

    ParsedItemData(ItemID id) : id(id)
    {
        this->reset();
    }
};

class ParsedItemDataList: public ObjectList<class ParsedItemData, ItemID>
{
    public:
        const char* name() { return "ItemLoadData"; }

    ParsedItemDataList(size_t capacity) :
        ObjectList<class ParsedItemData, ItemID>(capacity, NULL_ITEM)
    { this->print(); }
};


class ParsedItemData* create_item_data();
void destroy_item_data(ItemID id);
void clear_item_data();

int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container);

bool parse_item_token(const char* key, const char* val, class ParsedItemData* data);

bool create_container_items_from_data(ItemContainerID container_id);
bool create_player_container_items_from_data(AgentID agent_id, ItemContainerID* containers, size_t n_containers);

void init_items();
void teardown_items();

}   // serializer
