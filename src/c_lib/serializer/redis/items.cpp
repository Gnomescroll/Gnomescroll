#include "items.hpp"

#include <common/template/elastic_simple_object_list.hpp>
#include <item/common/enum.hpp>
#include <serializer/redis/redis.hpp>

// PER UNIT MODULE:
// PIPELINING   -- is default, with async
// MULTI/EXEC   -- is only needed when we have multiple redis clients accessing the same db.
    // at this point, we would batch everything in MULTIs with WATCH
    // ACTUALLY -- multi/exec is deprecated for the lua scripting. we will use transactional lua scripts at that point.
// ERROR HANDLING -- WRITING TO DISK
// LOADING DATA
// SAVE STATE INFO ON THE ITEM ITSELF (NEEDS_SAVING etc)

#include <item/common/constants.hpp>

namespace serializer
{
namespace redis
{

class PlayerItemSaveData
{
    public:
        int id;
        int user_id;
        ItemID item_id;
        bool remove_item_after;
        
    void reset()
    {
        this->user_id = 0;
        this->item_id = NULL_ITEM;
        this->remove_item_after = false;
    }

    void print()
    {
        printf("id: %d - user_id: %d - item_id: %d - remove: %d\n",
            this->id, this->user_id, this->item_id, this->remove_item_after); 
    }

    PlayerItemSaveData()
    : id(-1)
    {
        this->reset();
    }
};

// Double the max connections should be more than enough to avoid filling up this list
// The server would have to be full, everyone disconnect, reconnect, and disconnect, before redis replies at all
// Of course, redis could be down, or whatever so we still should handle errors

// TODO -- elastic
class PlayerItemSaveDataList: public ElasticObjectList<PlayerItemSaveData, 1024>
{
    public:
        const char* name() { return "PlayerItemSaveDataList"; }

    PlayerItemSaveDataList() { this->print(); }
};

// cache for player data passed around in the redis callbacks
static class PlayerItemSaveDataList* player_item_save_data_list = NULL;

// continuous, sequential array of all possible (local) ItemIDs (0 to MAX_ITEMS-1)
// used for the void *data data passed around in the redis callbacks 
static ItemID* item_id_cache = NULL;


void init_items()
{
    GS_ASSERT(item_id_cache == NULL);
    if (item_id_cache == NULL)
    {
        item_id_cache = (ItemID*)malloc(MAX_ITEMS * sizeof(ItemID));
        for (int i=0; i<MAX_ITEMS; i++)
            item_id_cache[i] = (ItemID)i;
    }

    GS_ASSERT(player_item_save_data_list == NULL);
    if (player_item_save_data_list == NULL)
        player_item_save_data_list = new PlayerItemSaveDataList;
}

void teardown_items()
{
    if (item_id_cache != NULL) delete item_id_cache;
    if (player_item_save_data_list != NULL) delete player_item_save_data_list;
}

static int save_item(class Item::Item* item, void* data); // forward decl

static void item_gid_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    redisReply* reply = (redisReply*)_reply;
    
    PlayerItemSaveData* data = (PlayerItemSaveData*)_data;

    class Item::Item* item = Item::get_item(data->item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;   // TODO -- log error
    GS_ASSERT(item->global_id == 0)
    if (item->global_id != 0) return;

    if (reply->type == REDIS_REPLY_INTEGER)
    {
        item->global_id = (int64_t)reply->integer;
        // schedule actual save now
        save_item(item, data);   // TODO -- handle error reply from this
    }
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(false);
        printf("Item gid acquisition failed with error: %s\n", reply->str);
    }
    else
    {
        GS_ASSERT(false);
        printf("Warning: unhandled redis reply type %d received for item_gid_cb\n", reply->type);
    }
}

static void item_save_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    redisReply* reply = (redisReply*)_reply;

    PlayerItemSaveData* data = (PlayerItemSaveData*)_data;
    class Item::Item* item = Item::get_item(data->item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL)
    {
        printf("Item not found for player item save: ");
        data->print();
    }

    if (reply->type == REDIS_REPLY_STATUS)
    {
        if (item != NULL)
        {
            item->save_state = ISS_SAVED;
            if (data->remove_item_after)
                Item::destroy_item(item->id);
        }
    }
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(false);
        printf("Item saving failed with error: %s\n", reply->str);
    }
    else
    {
        GS_ASSERT(false);
        printf("Unhandled reply received from redis for item saving\n");
    }

    player_item_save_data_list->destroy(data->id);
}

static bool make_item_gid(class Item::Item* item, void* data)
{
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return false;

    item->save_state = ISS_WAITING_FOR_GID;

    int ret = redisAsyncCommand(ctx, &item_gid_cb, data, "INCR item:gid");
    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

// Later, pipeline/multi the item save batches, for containers

static int save_item(class Item::Item* item, void* data)
{   // return -1 for error, 0 for success, 1 for wait
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return -1;

    if (item->global_id == 0)
    {
        bool ret = make_item_gid(item, data);
        if (ret) return 1;
        return -1;
    }

    const char* item_name = Item::get_item_name(item->type);
    GS_ASSERT(item_name != NULL);
    if (item_name == NULL) return -1;

    // TODO -- location_id must be translated to a global id for that type
    // This means we need to get a gid for containers when they are created
        // or when they are first serialized
    // We sort of need to save hands in case of crash? -- ignore for now.
        // later we can have a magical "lost items" container appear for handling items
        // that cant be fit in the container but need to be given back to the player

    int ret = redisAsyncCommand(ctx, &item_save_cb, data,
        "HMSET item:%lld %s %lld %s %s %s %d %s %d %s %d %s %d %s %d",
            item->global_id,
            "global_id",      item->global_id,
            "name",           item_name,
            "durability",     item->durability,
            "stack_size",     item->stack_size,
            "location",       item->location,
            "location_id",    item->location_id,
            "container_slot", item->container_slot);

    GS_ASSERT(ret == REDIS_OK);
    if (ret == REDIS_OK) return 0;
    return -1;
}

void save_player_container(int client_id, int container_id, bool remove_items_after)
{
    GS_ASSERT(player_item_save_data_list != NULL);
    if (player_item_save_data_list == NULL) return; // TODO -- log error
    
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return;    // TODO -- log error

    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return; // TODO -- log error
    NetPeerManager* npm = NetServer::clients[client_id];
    GS_ASSERT(npm != NULL);
    if (npm == NULL) return;    // TODO -- log error

    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;  // TODO -- log error

    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;

        class Item::Item* item = Item::get_item(container->slot[i]);
        GS_ASSERT(item != NULL);
        if (item == NULL) continue;

        class PlayerItemSaveData* data = player_item_save_data_list->create();
        GS_ASSERT(data != NULL);
        if (data == NULL) return;   // TODO -- log error
        data->user_id = npm->user_id;
        data->item_id = item->id;
        data->remove_item_after = remove_items_after;
        GS_ASSERT(data->item_id != NULL_ITEM);
        
        item->save_state = ISS_WAITING_FOR_SAVE;
        
        int ret = save_item(item, data);
        GS_ASSERT(ret >= 0);
        if (ret < 0) printf("Error saving item: %lld:%d\n", item->global_id, item->id);
    }
}

}   // redis
}   // serializer
