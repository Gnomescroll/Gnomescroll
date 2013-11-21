/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# include <common/subscriber_list.hpp>
# if GS_SERIALIZER
#  include <serializer/uuid.hpp>
# endif
#endif

namespace Item
{

#if DC_SERVER
void verify_items();
#endif

const int ITEM_SUBSCRIBER_LIST_INITIAL_SIZE = 1;
const int ITEM_SUBSCRIBER_LIST_HARD_MAX = 1;

class Item
{
    public:

        ItemID id;
        ItemType type;
        int32_t global_id;

        int durability;
        int stack_size;

        ItemLocationType location;
        int location_id;
        int container_slot;

        // if these item-specific properties get out of hand,
        // might need to switch to a void* with metadata struct
        // or generic fields
        int charges;
        #if DC_SERVER
        int recharge_tick;
        int gas_decay;
        #endif

        #if DC_SERVER
        SubscriberList<ClientID> subscribers;
        bool valid;
        #endif

    #if DC_SERVER && GS_SERIALIZER
        uuid_t uuid;

    void init_from_loading();   // only to be used by serializer
    #endif

    void init(ItemType item_type);

    void print()
    {
        printf("Item:\n");
        printf("ID %d\n", id);
        printf("Global ID: %d\n", this->global_id);
        printf("Group %d\n", get_item_group_for_type(this->type));
        printf("Type %d\n", type);
        printf("Durability %d\n", durability);
        printf("Stack size %d\n", stack_size);
        printf("Location %d\n", location);
        printf("Location ID %d\n", location_id);
        printf("Container slot %d\n", container_slot);
        printf("Charges %d\n", charges);
        #if DC_SERVER
        printf("Gas decay %d\n", this->gas_decay);
        printf("Recharge tick %d\n", this->recharge_tick);
        # if GS_SERIALIZER
        if (Options::serializer)
        {
            static char uuid_str[serializer::UUID_STRING_LENGTH+1];
            uuid_unparse(this->uuid, uuid_str);
            printf("UUID: %s", uuid_str);
        }
        # endif
        printf("Subscribers %lu\n", (long unsigned)subscribers.count);
        printf("\t");
        for (size_t i=0; i<subscribers.count; i++) printf("%d ", subscribers.subscribers[i]);
        printf("\n");
        #endif
    }

    explicit Item(ItemID id) :
        id(id),
        type(NULL_ITEM_TYPE),
        global_id(0),
        durability(NULL_DURABILITY),
        stack_size(1),
        location(IL_NOWHERE),
        location_id(NULL_LOCATION),
        container_slot(NULL_SLOT),
        charges(NULL_CHARGES)
        #if DC_SERVER
        , recharge_tick(0)
        , gas_decay(NULL_GAS_LIFETIME)
        , subscribers(ITEM_SUBSCRIBER_LIST_INITIAL_SIZE, ITEM_SUBSCRIBER_LIST_HARD_MAX)
        , valid(true)
        #endif
    {
        #if DC_SERVER && GS_SERIALIZER
        if (Options::serializer)
            uuid_clear(this->uuid);
        #endif
    }

};

}   // Item

#include <common/template/object_list.hpp>

namespace Item
{

class ItemList: public ObjectList<Item, ItemID>
{
    private:
        #if DC_SERVER
        unsigned int gas_tick;
        static const int GAS_TICK_INTERVAL = 10;
        #endif

    const char* name()
    {
        return "Item";
    }

    public:
    ItemList(size_t capacity) :
        ObjectList<Item, ItemID>(capacity, NULL_ITEM)
        #if DC_SERVER
        , gas_tick(0)
        #endif
    {}

    #if DC_CLIENT && !PRODUCTION
    Item* create()
    {
        GS_ASSERT(false);
        printf("Must create item with id\n");
        return NULL;
    }
    #endif

    #if DC_CLIENT
    Item* create_type(ItemType item_type, ItemID item_id)
    {
        Item* item = ObjectList<Item, ItemID>::create(item_id);
        if (item == NULL) return NULL;
        item->init(item_type);
        return item;
    }
    #endif

    #if DC_SERVER
    Item* create_type(ItemType item_type)
    {
        Item* item = this->create();
        IF_ASSERT(item == NULL) return NULL;
        item->init(item_type);
        return item;
    }

    void decay_gas();
    void recharge_items();
    void verify_items();
    #endif
};

}   // Item
