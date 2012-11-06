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
bool is_valid_location_data(ItemLocationType location, int location_id, int container_slot, const int assert_limit);
#endif

const int ITEM_SUBSCRIBER_LIST_INITIAL_SIZE = 1;
const int ITEM_SUBSCRIBER_LIST_HARD_MAX = 1;

class Item
{
    public:

        ItemID id;
        int type;  // stone_block, dirt_block, mining_laser_beta,
        int32_t global_id;

        int durability;
        int stack_size;

        ItemLocationType location;
        int location_id;
        int container_slot;

        int gas_decay;

        #if DC_SERVER
        SubscriberList<ClientID> subscribers;
        bool valid;
        #endif

    #if DC_SERVER && GS_SERIALIZER
    uuid_t uuid;
    void init_from_loading();   // only to be used by serializer
    #endif
    
    void init(int item_type);
    
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
        printf("Gas decay %d\n", gas_decay);
        #if DC_SERVER
        # if GS_SERIALIZER
        if (Options::serializer)
        {
            static char uuid_str[(serializer::UUID_STRING_LENGTH)+1];
            uuid_unparse(this->uuid, uuid_str);
            printf("UUID: %s", uuid_str);
        }
        # endif
        printf("Subscribers %d\n", subscribers.count);
        printf("\t");
        for (unsigned int i=0; i<subscribers.count; i++) printf("%d ", subscribers.subscribers[i]);
        printf("\n");
        #endif
    }

    explicit Item(ItemID id)
    :   id(id),
        type(NULL_ITEM_TYPE),
        global_id(0),
        durability(NULL_DURABILITY),
        stack_size(1),
        location(IL_NOWHERE),
        location_id(NULL_LOCATION),
        container_slot(NULL_SLOT),
        gas_decay(NULL_GAS_LIFETIME)
        #if DC_SERVER
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

}

#include <common/template/object_list.hpp>

namespace Item
{

class ItemList: public ObjectList<Item, ItemID>
{
    private:
        const char* name() { return "Item"; }

    public:
        ItemList(unsigned int capacity) : ObjectList<Item, ItemID>(capacity, NULL_ITEM),
            gas_tick(0)
        {
            this->print();
        }

        #if DC_CLIENT && !PRODUCTION
        Item* create()
        {
            printf("must create item with id\n");
            GS_ASSERT(false);
            return NULL;
        }
        #endif

        #if DC_CLIENT
        Item* create_type(int item_type, ItemID item_id)
        {
            Item* item = ObjectList<Item, ItemID>::create(item_id);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        #endif

        #if DC_SERVER
        Item* create_type(int item_type)
        {
            Item* item = this->create();
            GS_ASSERT(item != NULL);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        #endif

    private:
        unsigned int gas_tick;
        static const int GAS_TICK_INTERVAL = 10;
    public:
        #if DC_SERVER
        void decay_gas();
        void verify_items();
        #endif
};

}   // Item
