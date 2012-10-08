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
        UserID user_id;
        ItemID item_id;
        bool remove_item_after;
        LocationNameID location_name_id;
        
    PlayerItemSaveData() :
        id(-1), user_id(NULL_USER_ID), item_id(NULL_ITEM),
        remove_item_after(false), location_name_id(LN_NONE)
    {}
};

class PlayerItemLoadData
{
    public:
        int id;
        UserID user_id;
        int container_id;

    PlayerItemLoadData() :
        id(-1), user_id(NULL_USER_ID), container_id(NULL_CONTAINER)
    {}
};

class ItemLoadData
{
    public:
        int id;
        int64_t item_guid;

    ItemLoadData() :
        id(-1), item_guid(0)
    {}
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

class PlayerItemLoadDataList: public ElasticObjectList<PlayerItemLoadData, NetServer::HARD_MAX_CONNECTIONS>
{
    public:
        const char* name() { return "PlayerItemLoadDataList"; }

    PlayerItemLoadDataList() :
     ElasticObjectList<PlayerItemLoadData, NetServer::HARD_MAX_CONNECTIONS>(PLAYER_ITEM_LOAD_DATA_LIST_HARD_MAX)
     { this->print(); }
};

class ItemLoadDataList: public ElasticObjectList<ItemLoadData, 1024>
{
    public:
        const char* name() { return "ItemLoadDataList"; }

    ItemLoadDataList() :
     ElasticObjectList<ItemLoadData, 1024>(ITEM_LOAD_DATA_LIST_HARD_MAX)
     { this->print(); }
};

// cache for player data passed around in the redis callbacks
static class PlayerItemSaveDataList* player_item_save_data_list = NULL;
static class PlayerItemLoadDataList* player_item_load_data_list = NULL;
static class ItemLoadDataList* item_load_data_list = NULL;

void load_item(int64_t item_guid);    // forward decl

void init_items()
{
    GS_ASSERT(player_item_save_data_list == NULL);
    player_item_save_data_list = new class PlayerItemSaveDataList;
    
    GS_ASSERT(player_item_load_data_list == NULL);
    player_item_load_data_list = new class PlayerItemLoadDataList;
    
    GS_ASSERT(item_load_data_list == NULL);
    item_load_data_list = new class ItemLoadDataList;
}

void teardown_items()
{
    if (player_item_save_data_list != NULL)
    {
        GS_ASSERT(player_item_save_data_list->ct == 0);
        delete player_item_save_data_list;
    }
    if (player_item_load_data_list != NULL)
    {
        GS_ASSERT(player_item_load_data_list->ct == 0);
        delete player_item_load_data_list;
    }
    if (item_load_data_list != NULL)
    {
        GS_ASSERT(item_load_data_list->ct == 0);
        delete item_load_data_list;
    }
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

    class PlayerItemSaveData* data = (class PlayerItemSaveData*)_data;
    class Item::Item* item = Item::get_item(data->item_id);
    GS_ASSERT(item != NULL);

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

static void load_player_container_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    // TODO -- need to mark the container that its done loading items
    
    redisReply* reply = (redisReply*)_reply;
    class PlayerItemLoadData* data = (class PlayerItemLoadData*)_data;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        // we could MULTI/EXEC here but its not necessary
        char* endptr = NULL;
        for (unsigned int i=0; i<reply->elements; i++)
        {
            redisReply* subreply = reply->element[i];
            GS_ASSERT(subreply->type == REDIS_REPLY_STRING);
            if (subreply->type != REDIS_REPLY_STRING) continue;
            int64_t item_guid = (int64_t)strtoll(subreply->str, &endptr, 10);
            GS_ASSERT(subreply->str[0] != '\0' && endptr[0] == '\0');
            if (subreply->str[0] != '\0' && endptr[0] == '\0')
                load_item(item_guid);
            //else
                // log error, parsing failed
        }
    }
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(false);
        printf("Player container loading failed with error: %s\n", reply->str);
    }
    else
    {
        GS_ASSERT(false);
        printf("Unhandled reply received from redis for player container loading\n");
    }

    player_item_load_data_list->destroy(data->id);
}

static void load_item_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    // TODO -- when destroy the tmp item due to failure,
    // we either need to clear its location data to avoid triggering the autocleanup
    // or have a separate destruction method (better)
    
    redisReply* reply = (redisReply*)_reply;
    class ItemLoadData* data = (class ItemLoadData*)_data;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        GS_ASSERT(reply->elements == EXPECTED_ITEM_HASH_FIELDS*2);
        bool err = false;
        char* location_name = NULL;
        int location_id = NULL_LOCATION;

        // TODO -- item list should have enough reserved space to load everyone's inventories
        Item::Item* item = Item::create_item_for_loading();
        item->save_state = ISS_LOADING;
        GS_ASSERT(item != NULL);
        if (item != NULL)
        {
            printf("%d elements\n", reply->elements);
            for (unsigned int i=0; i<reply->elements; i+=2)
            {
                GS_ASSERT(reply->element[i]->type == REDIS_REPLY_STRING);
                GS_ASSERT(reply->element[i+1]->type == REDIS_REPLY_STRING);
                if (reply->element[i]->type != REDIS_REPLY_STRING
                 || reply->element[i+1]->type != REDIS_REPLY_STRING)
                {
                    err = true;
                    break;
                }
                char* key = reply->element[i]->str;
                char* value = reply->element[i+1]->str;

                printf("Key: %s\n", key);
                printf("Value: %s\n", value);

                // create an item from the data

                if (strcmp(key, ITEM_GUID_KEYNAME) == 0)
                    item->global_id = (int64_t)parse_int(value, err);
                else
                if (strcmp(key, ITEM_NAME_KEYNAME) == 0)
                {
                    item->type = Item::get_versioned_item_type(value);
                    if (item->type == NULL_ITEM_TYPE) err = true;
                }
                else
                if (strcmp(key, ITEM_DURABILITY_KEYNAME) == 0)
                    item->durability = (int)parse_int(value, err);
                else
                if (strcmp(key, ITEM_STACK_SIZE_KEYNAME) == 0)
                    item->stack_size = (int)parse_int(value, err);
                else
                if (strcmp(key, ITEM_LOCATION_KEYNAME) == 0)
                    location_name = value;
                else
                if (strcmp(key, ITEM_LOCATION_ID_KEYNAME) == 0)
                    location_id = (int)parse_int(value, err);
                else
                if (strcmp(key, ITEM_CONTAINER_SLOT_KEYNAME) == 0)
                    item->container_slot = (int)parse_int(value, err);
                else
                    err = true;

                GS_ASSERT(!err);

                if (err) break;
            }

            GS_ASSERT(location_name != NULL);
            GS_ASSERT(location_id != NULL_LOCATION);

            // TODO --
            if (err || location_name == NULL || location_id == NULL_LOCATION)
            {
                Item::destroy_item(item->id);
                //log_error();  // TODO
                goto unload;
            }

            // if location name starts with "player"
                // if location is hand, IL_HAND and get hand id for player (location_id)
                // else, IL_CONTAINER and get id for container matching the string
            // else if location name is container
                // location_id is that container's guid (we need to have the global containers loaded by this point)
                    // since containers are only grabbed on load, it is ok to repeatedly iterate the container list to find guid
            // else if location name is particle    [[ NOT YET SUPPORTED -- isnt needed unless we want it later.]
                // location id is guid for the particle. same idea as for the container loading

            if (str_starts_with(location_name, PLAYER_CONTAINER_LOCATION_PREFIX))
            {
                // interpret location_id as the uuid of the player
                NetPeerManager* client = NetServer::get_client_from_user_id((UserID)location_id);
                GS_ASSERT(client != NULL);  // This can happen under normal conditions, e.g. player joins and quits immediately 
                if (client == NULL)
                {
                    Item::destroy_item(item->id);
                    // log error
                    goto unload;
                }
                
                char* subloc = &location_name[sizeof(PLAYER_CONTAINER_LOCATION_PREFIX)-1];
                if (strcmp(subloc, PLAYER_HAND_LOCATION_SUBNAME) == 0)
                {
                    item->location_id = ItemContainer::get_agent_hand(client->agent_id);
                    item->location = IL_HAND;
                }
                else
                {
                    item->location = IL_CONTAINER;
                    if (strcmp(subloc, PLAYER_INVENTORY_LOCATION_SUBNAME) == 0)
                        item->location_id = ItemContainer::get_agent_inventory(client->agent_id);
                    else
                    if (strcmp(subloc, PLAYER_ENERGY_TANKS_LOCATION_SUBNAME) == 0)
                        item->location_id = ItemContainer::get_agent_energy_tanks(client->agent_id);
                    else
                    if (strcmp(subloc, PLAYER_SYNTHESIZER_LOCATION_SUBNAME) == 0)
                        item->location_id = ItemContainer::get_agent_synthesizer(client->agent_id);
                    else
                    if (strcmp(subloc, PLAYER_TOOLBELT_LOCATION_SUBNAME) == 0)
                        item->location_id = ItemContainer::get_agent_toolbelt(client->agent_id);
                }
            }
            else
            if (strcmp(CONTAINER_LOCATION_NAME, location_name) == 0)
                item->location = IL_CONTAINER;   // TODO -- lookup container id from location_id (which should be container guid)
            else
            if (strcmp(PARTICLE_LOCATION_NAME, location_name) == 0)
                item->location = IL_PARTICLE;    // TODO -- look up particle

            err = !(item->valid_deserialization());
            GS_ASSERT(!err);
            if (err)
            {
                Item::destroy_item(item->id);
                //log_error();    // TODO
                goto unload;
            }

            // need to place the item into the container it should be in
            err = !(ItemContainer::load_item_into_container(item->id));

            GS_ASSERT(!err)
            if (err)
            {
                Item::destroy_item(item->id);
                // TODO -- log error
                goto unload;
            }

            err = !(item->init_for_loading());

            GS_ASSERT(!err);
            if (err)
            {
                Item::destroy_item(item->id);
                // TODO -- log error
                goto unload;
            }
            
            item->save_state = ISS_LOADED;
        }
    }
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(false);
        printf("Item %lld loading from redis failed with error: %s\n", data->item_guid, reply->str);
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
    {
        GS_ASSERT(false);
        printf("Item %lld not found\n", data->item_guid);
    }
    else
    {
        GS_ASSERT(false);
        printf("Unhandled reply received from redis for item %lld loading", data->item_guid);
    }

    unload:
        item_load_data_list->destroy(data->id);
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
        "%s %d "    // location id     (meaning is depedent on location_name. for player it is player id. for container it is container id. for particle it is position (server id maybe later -- might be separate field))
        "%s %d",    // container slot (only relevant if container)
        item->global_id,
        ITEM_GUID_KEYNAME,           item->global_id,
        ITEM_NAME_KEYNAME,           item_name,
        ITEM_DURABILITY_KEYNAME,     item->durability,
        ITEM_STACK_SIZE_KEYNAME,     item->stack_size,
        ITEM_LOCATION_KEYNAME,       location_name,
        ITEM_LOCATION_ID_KEYNAME,    data->user_id,
        ITEM_CONTAINER_SLOT_KEYNAME, item->container_slot);
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

    class NetPeerManager* client = NetServer::get_client(client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL) return;    // TODO -- log error
    GS_ASSERT(client->user_id != NULL_USER_ID);
    if (client->user_id == NULL_USER_ID) return;

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
        data->user_id = client->user_id;
        data->item_id = item->id;
        data->remove_item_after = remove_items_after;
        data->location_name_id = get_player_location_name_id(container->type);
        GS_ASSERT(data->item_id != NULL_ITEM);
        
        int ret = save_player_item(item, location_name_id, data);
        GS_ASSERT(ret >= 0);
        if (ret < 0) printf("Error saving item: %lld:%d\n", item->global_id, item->id);
    }
}

void load_player_container(int client_id, int container_id)
{
    NetPeerManager* client = NetServer::get_client(client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL) return;
    GS_ASSERT(client->user_id != NULL_USER_ID);
    if (client->user_id == NULL_USER_ID) return;

    ItemContainerType container_type = ItemContainer::get_container_type(container_id);
    LocationNameID loc_id = get_player_location_name_id(container_type);
    const char* location_name = get_location_name(loc_id);
    GS_ASSERT(location_name != NULL);
    if (location_name == NULL) return;
    
    class PlayerItemLoadData* data = player_item_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return;
    data->user_id = client->user_id;
    data->container_id = container_id;

    int ret = redisAsyncCommand(ctx, &load_player_container_cb, data,
        "SMEMBERS %s:%d", location_name, client->user_id);
    GS_ASSERT(ret == REDIS_OK);
}

void load_item(int64_t item_guid)
{
    // no data packet needed; all information should be in the hash
    // TODO -- dont "agent_born" the containers until they are fully loaded!
    // this means waiting for all data responses about the items found in the container

    class ItemLoadData* data = item_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return;   // TODO -- log error
    data->item_guid = item_guid;
    
    int ret = redisAsyncCommand(ctx, &load_item_cb, data,
        "HGETALL item:%lld", item_guid);
    GS_ASSERT(ret == REDIS_OK);
}

}   // serializer
