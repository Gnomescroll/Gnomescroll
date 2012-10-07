#include "items.hpp"

#include <common/template/elastic_simple_object_list.hpp>
#include <item/common/enum.hpp>
#include <serializer/constants.hpp>
#include <serializer/redis.hpp>

// PER UNIT MODULE:
// PIPELINING   -- is default, with async
// MULTI/EXEC   -- is only needed when we have multiple redis clients accessing the same db.
    // at this point, we would batch everything in MULTIs with WATCH
    // ACTUALLY -- multi/exec is deprecated for the lua scripting. we will use transactional lua scripts at that point.
// ERROR HANDLING -- WRITING TO DISK
// LOADING DATA
// SAVE STATE INFO ON THE ITEM ITSELF (NEEDS_SAVING etc)

/*
 *  Container/player item contents Serialization technique options:
 *      Save on every container transaction -- start with this
 *      Save in bulk, periodically (if changed at all)
 */ 

#include <item/common/constants.hpp>

namespace serializer
{

// Saving a player:
// Save all player metadata
// Save all container items from player
// After all items in player's container have their gids (check periodically)
// Then add the gids to a set for player
    // Unfortunately, cannot attach a set to a player hash key
    // So need a separate key
    // player:items:<user_id>   -- 
    // player:<user_id> -- hash. we need to add a hashkey to the player's items set key, if we want to rename things

// We're going to need separate lists for items, containers and clients/agents, while they are being saved?
    // Yes. Additionally, if they are to be removed after saving, we can simply remove them from the pool when copied to the save buffer
    // There are some issues with the periodic saving; we have to make sure to only remove the reference of item in main pool, if its current in the save pool,
        // and the save pool needs to know so that it will delete it

// Container saving:
    // First, the container needs a gid
    // Then, we need to save its contents as normal, collecting all gids (use the save data struct to help keep track)
    // Once all ids are acquired, then we can save the ids to the set for the container

// Loading Player
    // HMGET his data
    // get his set of inventory ids
        // PROBLEM: we cannot hash to a set, but we need a way to encode each container he has
            // SOLUTIONS:

                // One key per container type per player, mapping to the set
                    // --REQ: N extra keys per player
                    
                // When saving an item, and location is IL_CONTAINER, and container is attached_to_agent,
                    // save location_id as the type of container it is, for the agent
                    // --REQ: extra processing iin the server save/load

                // go with the 2nd one, as it is more compact.

                    // on save, translate IL_CONTAINER to IL_PLAYER
                    // translate location_id to the type of container it is
                    
                    // on load, check that it is IL_PLAYER (we will be expecting it, as we should only load it for the player)
                    // change location to IL_CONTAINER
                    // change location_id to the id of that container type attached to the agent

// DATA THAT CANNOT BE CHANGED -- must use strings, and a renaming scheme for names that change
        // Container types
        // Item types
        // Item.location type

class PlayerItemSaveData
{
    public:
        int id;
        int user_id;
        ItemID item_id;
        bool remove_item_after;
        LocationNameID location_name_id;
        
    void reset()
    {
        this->user_id = 0;
        this->item_id = NULL_ITEM;
        this->remove_item_after = false;
        this->location_name_id = LN_NONE;
    }

    void print()
    {
        printf("id: %d - user_id: %d - item_id: %d - remove: %d - location_name - %s\n",
            this->id, this->user_id, this->item_id, this->remove_item_after, get_location_name(this->location_name_id)); 
    }

    PlayerItemSaveData(const PlayerItemSaveData& other)
    {
        this->user_id = other.user_id;
        this->item_id = other.item_id;
        this->remove_item_after = other.remove_item_after;
        this->location_name_id = other.location_name_id;
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

class PlayerItemSaveDataList: public ElasticObjectList<PlayerItemSaveData, 1024>
{
    public:
        const char* name() { return "PlayerItemSaveDataList"; }

    PlayerItemSaveDataList() :
     ElasticObjectList<PlayerItemSaveData, 1024>(PLAYER_ITEM_SAVE_DATA_LIST_HARD_MAX)
     { this->print(); }
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
    if (item_id_cache != NULL) free(item_id_cache);
    if (player_item_save_data_list != NULL) delete player_item_save_data_list;
}

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
        save_player_item(item, data->location_name_id, data);   // TODO -- handle error reply from this
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

static void player_item_add_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    //getCallback(ctx, _reply, _data);
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

int save_player_item(class Item::Item* item, LocationNameID location_name_id, class PlayerItemSaveData* data)
{   // return -1 for error, 0 for success, 1 for wait
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return -1;

    // TODO -- handle case where we attempt to save while waiting for a save
    GS_ASSERT(item->save_state != ISS_WAITING_FOR_SAVE);

    if (item->global_id == 0)
    {
        bool ret = make_item_gid(item, data);
        if (ret) return 1;
        return -1;
    }

    item->save_state = ISS_WAITING_FOR_SAVE;

    const char* item_name = Item::get_item_name(item->type);
    GS_ASSERT(item_name != NULL);
    if (item_name == NULL) return -1;

    const char* location_name = get_location_name(location_name_id);
    GS_ASSERT(location_name != NULL);
    if (location_name == NULL) return -1;

    #define CHECK_REDIS_OK(ret)\
    do \
    { \
        GS_ASSERT(ret == REDIS_OK); \
        if (ret != REDIS_OK) \
        { \
            redisAsyncCommand(ctx, NULL, NULL, "DISCARD"); \
            return -1; \
        } \
    } while(0);

    /*
     * MULTI
     * HMSET item:<guid> etc
     * SADD player:<container_name>:<user_id> <item_guid>
     * EXEC
     */

    int ret = REDIS_OK;

    ret = redisAsyncCommand(ctx, NULL, NULL, "MULTI");
    CHECK_REDIS_OK(ret);

    ret = redisAsyncCommand(ctx, &item_save_cb, data,
        "HMSET item:%lld " // key
        "%s %lld "  // global id
        "%s %s "    // item name
        "%s %d "    // durability
        "%s %d "    // stack size
        "%s %s "    // location name   ("player:[hand,inventory,energy_tanks,etc} [container attached to agent]","container", "particle")
        "%s %d "    // location id     (meaning is depedent on location_name. for player it is player id. for container it is container id. for particle it is nothing (server id maybe later -- might be separate field))
        "%s %d",    // container slot (only relevant if container)
            item->global_id,
            "global_id",      item->global_id,
            "name",           item_name,
            "durability",     item->durability,
            "stack_size",     item->stack_size,
            "location",       location_name,
            "location_id",    data->user_id,
            "container_slot", item->container_slot);
    CHECK_REDIS_OK(ret);

    ret = redisAsyncCommand(ctx, &player_item_add_cb, data,
        "SADD %s:%d %lld", location_name, data->user_id, item->global_id);
    CHECK_REDIS_OK(ret);

    ret = redisAsyncCommand(ctx, NULL, NULL, "EXEC");
    CHECK_REDIS_OK(ret);

    #undef CHECK_REDIS_OK

    return 0;
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
    GS_ASSERT(container->attached_to_agent);
    if (!container->attached_to_agent) return;  // TODO -- log error

    LocationNameID location_name_id = get_player_location_name_id(container->type);
    
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
        data->location_name_id = get_player_location_name_id(container->type);
        GS_ASSERT(data->item_id != NULL_ITEM);
        
        int ret = save_player_item(item, location_name_id, data);
        GS_ASSERT(ret >= 0);
        if (ret < 0) printf("Error saving item: %lld:%d\n", item->global_id, item->id);
    }
}

}   // serializer
