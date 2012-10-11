#include "items.hpp"

#include <item/item.hpp>
#include <item/common/constants.hpp>
#include <serializer/redis.hpp>

namespace serializer
{

// cache for player data passed around in the redis callbacks
static class PlayerLoadDataList* player_load_data_list = NULL; 
static class PlayerItemLoadDataList* player_item_load_data_list = NULL;
static class PlayerContainerLoadDataList* player_container_load_data_list = NULL;
static class PlayerItemSaveDataList* player_item_save_data_list = NULL;

void load_player_item(int64_t item_guid, class PlayerContainerLoadData* container_data);    // forward decl

class PlayerItemSaveData
{
    public:
        int id;

        UserID user_id;
        
        LocationNameID location_name_id;

        // copy of the item data
        ItemID item_id;
        int64_t global_id;
        int item_type;
        int durability;
        int stack_size;
        ItemLocationType location;
        int location_id;
        int container_slot;

        bool was_removed;   // dont expect to find the Item* obj later 
                
    PlayerItemSaveData() :
        id(-1), user_id(NULL_USER_ID), location_name_id(LN_NONE),
        item_id(NULL_ITEM), global_id(0), item_type(NULL_ITEM_TYPE),
        durability(NULL_DURABILITY), stack_size(NULL_STACK_SIZE),
        location(IL_NOWHERE), location_id(NULL_LOCATION), container_slot(NULL_SLOT),
        was_removed(false)
    {}
};

class PlayerLoadData
{
    private:
    bool signal_if_loaded()
    {
        GS_ASSERT(!this->waiting_for_setup);
        if (this->waiting_for_setup) return false;
        if (!this->player_data_loaded) return false;
        for (int i=0; i<N_PLAYER_CONTAINERS; i++)
            if (!this->containers_loaded[i]) return false;

        NetPeerManager* client = NetServer::get_client(this->client_id);
        GS_ASSERT(client != NULL);
        if (client == NULL) return false; // TODO -- log error
        client->was_deserialized();
        // TODO -- destroy ourself here
        return true;
    }
        
    public:
        int id;
        UserID user_id;
        int client_id;

        bool waiting_for_setup; // this object is setup in multiple steps, we need to indicate when we're done

        // loaded state
        bool player_data_loaded;

        int n_containers_expected;
        int containers_expected[N_PLAYER_CONTAINERS];
        bool containers_loaded[N_PLAYER_CONTAINERS];

    void setup_complete()
    {
        GS_ASSERT(this->waiting_for_setup);
        this->waiting_for_setup = false;
    }

    void expect_container(int container_id)
    {
        GS_ASSERT(this->n_containers_expected < N_PLAYER_CONTAINERS);
        if (this->n_containers_expected >= N_PLAYER_CONTAINERS) return;
        this->containers_expected[this->n_containers_expected++] = container_id;
    }

    bool container_was_loaded(int container_id)
    {
        for (int i=0; i<this->n_containers_expected; i++)
            if (this->containers_expected[i] == container_id)
            {
                GS_ASSERT(!this->containers_loaded[i]);
                this->containers_loaded[i] = true;
                return this->signal_if_loaded();
            }
        return false;
    }

    bool player_data_was_loaded()
    {
        GS_ASSERT(!this->player_data_loaded);
        this->player_data_loaded = true;
        return this->signal_if_loaded();
    }

    PlayerLoadData() :
        id(-1), user_id(NULL_USER_ID), client_id(NULL_CLIENT),
        waiting_for_setup(true),
        player_data_loaded(false), n_containers_expected(0)
    {
        for (int i=0; i<N_PLAYER_CONTAINERS; i++)
        {
            this->containers_expected[i] = NULL_CONTAINER;
            this->containers_loaded[i] = false;
        }
    }
};

class PlayerContainerLoadData
{
    public:
        int id;
        int container_id;
        unsigned int item_count;
        int player_data_id;

        void items_loaded()
        {
            class PlayerLoadData* player_data = player_load_data_list->get(this->player_data_id);
            GS_ASSERT(player_data != NULL);
            if (player_data == NULL) return;    // TODO -- log error
            if (player_data->container_was_loaded(this->container_id))
                player_load_data_list->destroy(this->player_data_id);
        }

    PlayerContainerLoadData() :
        id(-1), container_id(NULL_CONTAINER), item_count(0), player_data_id(-1)
    {}
};

class PlayerItemLoadData
{
    public:
        int id;
        int64_t item_guid;
        int container_data_id;

    PlayerItemLoadData() :
        id(-1), item_guid(0), container_data_id(-1)
    {}
};


void init_items()
{
    GS_ASSERT(player_load_data_list == NULL);
    player_load_data_list = new class PlayerLoadDataList;
        
    GS_ASSERT(player_item_load_data_list == NULL);
    player_item_load_data_list = new class PlayerItemLoadDataList;

    GS_ASSERT(player_container_load_data_list == NULL);
    player_container_load_data_list = new class PlayerContainerLoadDataList;

    GS_ASSERT(player_item_save_data_list == NULL);
    player_item_save_data_list = new class PlayerItemSaveDataList;
}

void teardown_items()
{
    // TODO -- log all unfinished cached objects, indicates failure
    if (player_load_data_list != NULL)
    {
        GS_ASSERT(player_load_data_list->ct == 0);
        delete player_load_data_list;
    }
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
    if (player_container_load_data_list != NULL)
    {
        GS_ASSERT(player_container_load_data_list->ct == 0);
        delete player_container_load_data_list;
    }
}

static void item_gid_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    redisReply* reply = (redisReply*)_reply;
    
    PlayerItemSaveData* data = (PlayerItemSaveData*)_data;

    class Item::Item* item = Item::get_item(data->item_id);
    GS_ASSERT(data->was_removed || item != NULL);
    if (!data->was_removed && item == NULL) return;   // TODO -- log error
    GS_ASSERT(item->global_id == 0)
    if (item->global_id != 0) return;
    
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        data->global_id = (int64_t)reply->integer;
        if (!data->was_removed)
            item->global_id = data->global_id;
        // schedule actual save now
        save_player_item(data);   // TODO -- handle error reply from this
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

    if (reply->type == REDIS_REPLY_STATUS)
    {
        class Item::Item* item = Item::get_item(data->item_id);
        if (item != NULL)
            item->save_state = ISS_SAVED;
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
    class PlayerContainerLoadData* data = (class PlayerContainerLoadData*)_data;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        // we could MULTI/EXEC here but its not necessary
        char* endptr = NULL;
        data->item_count = reply->elements;
        for (unsigned int i=0; i<reply->elements; i++)
        {
            redisReply* subreply = reply->element[i];
            GS_ASSERT(subreply->type == REDIS_REPLY_STRING);
            if (subreply->type != REDIS_REPLY_STRING)
            {
                data->item_count--;
                continue; // ERROR
            }
            int64_t item_guid = (int64_t)strtoll(subreply->str, &endptr, 10);
            GS_ASSERT(subreply->str[0] != '\0' && endptr[0] == '\0');
            if (subreply->str[0] != '\0' && endptr[0] == '\0')
                load_player_item(item_guid, data);
            else
                data->item_count--; // TODO -- log error
        }
        GS_ASSERT(data->item_count >= 0);
        if (data->item_count <= 0)
        {
            data->items_loaded();
            player_container_load_data_list->destroy(data->id);
        }
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
    {
        data->items_loaded();
        player_container_load_data_list->destroy(data->id);
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
        printf("Unhandled reply type %d received from redis for player container loading\n", reply->type);
    }
    // dont destroy the container data -- the item callback loader will
    // destroy the container data upon the last item being loaded
}

// returns false on error
static bool handle_item_hash_reply(redisReply* reply, class Item::Item* item)
{
    GS_ASSERT(reply->elements == EXPECTED_ITEM_HASH_FIELDS*2);
    bool err = false;
    char* location_name = NULL;
    int location_id = NULL_LOCATION;

    // TODO -- item list should have enough reserved space to load everyone's inventories
    item->save_state = ISS_LOADING;

    for (unsigned int i=0; i<reply->elements; i+=2)
    {
        char* key = reply->element[i]->str;
        char* value = reply->element[i+1]->str;

        // create an item from the data

        if (strcmp(key, ITEM_GUID_KEYNAME) == 0)
            item->global_id = (int64_t)parse_int(value, err);
        else
        if (strcmp(key, ITEM_NAME_KEYNAME) == 0)
        {
            item->type = Item::get_versioned_item_type(value);
            GS_ASSERT(item->type != NULL_ITEM_TYPE);
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
        return false;

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
        if (client == NULL) return false;
        
        char* subloc = &location_name[sizeof(PLAYER_CONTAINER_LOCATION_PREFIX)-1];
        if (strcmp(subloc, PLAYER_HAND_LOCATION_SUBNAME) == 0)
        {
            item->location_id = client->agent_id;
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
    if (err) return false;

    // need to place the item into the container it should be in
    if (item->location == IL_HAND)
        err = !(ItemContainer::load_item_into_hand(item->id, item->location_id));
    else
    if (item->location == IL_CONTAINER)
        err = !(ItemContainer::load_item_into_container(item->id, item->location_id, item->container_slot));
    else
    {   // TODO -- implement particle creation
        GS_ASSERT(false);
        err = true;
    }

    GS_ASSERT(!err)
    if (err) return false;

    item->init_for_loading();
    item->save_state = ISS_LOADED;

    return true;
}

static void load_player_item_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    // TODO -- when destroy the tmp item due to failure,
    // we either need to clear its location data to avoid triggering the autocleanup
    // or have a separate destruction method (better)
    
    redisReply* reply = (redisReply*)_reply;
    class PlayerItemLoadData* data = (class PlayerItemLoadData*)_data;
    GS_ASSERT(data->container_data_id >= 0);
    if (data->container_data_id < 0) return;  // TODO -- log error
    class PlayerContainerLoadData* container_data = player_container_load_data_list->get(data->container_data_id);
    GS_ASSERT(container_data != NULL);
    if (container_data == NULL) return; // TODO -- log error
    GS_ASSERT(container_data->item_count > 0);
    if (container_data->item_count <= 0) return;    // TODO -- log error

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        class Item::Item* item = Item::create_item_for_loading();
        GS_ASSERT(item != NULL);
        if (item != NULL && !handle_item_hash_reply(reply, item))
            Item::destroy_item_for_loading(item->id);
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

    container_data->item_count--;
    if (container_data->item_count <= 0)
    {   // this container is loaded, we are ready
        container_data->items_loaded();
        player_container_load_data_list->destroy(container_data->id);
    }
    player_item_load_data_list->destroy(data->id);
}

static void load_player_data_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    redisReply* reply = (redisReply*)_reply;
    class PlayerLoadData* data = (class PlayerLoadData*)_data;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        // TODO -- process player data here
        // suggested data to do soon: spawn points
        if (data->player_data_was_loaded())
            player_load_data_list->destroy(data->id);
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
    {
        if (data->player_data_was_loaded())
            player_load_data_list->destroy(data->id);
    }
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(false);
        printf("Loading player %d data failed with error: %s\n", data->user_id, reply->str);
    }
    else
    {
        GS_ASSERT(false);
        printf("Unhandled reply received from redis for player %d loading\n", data->user_id);
    }
}


static bool make_item_gid(class PlayerItemSaveData* data)
{
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return false;

    if (!data->was_removed)
    {
        class Item::Item* item = Item::get_item(data->item_id);
        GS_ASSERT(item != NULL);
        if (item != NULL)
            item->save_state = ISS_WAITING_FOR_GID;
    }

    int ret = redisAsyncCommand(ctx, &item_gid_cb, data, "INCR item:gid");
    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

int save_player_item(class PlayerItemSaveData* data)
{   // return -1 for error, 0 for success, 1 for wait
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL) return -1;

    class Item::Item* item = NULL;
    if (!data->was_removed)
    {
        item = Item::get_item(data->item_id);
        GS_ASSERT(item != NULL && item->save_state != ISS_WAITING_FOR_SAVE);
    }

    if (data->global_id == 0)
    {
        bool ret = make_item_gid(data);
        if (ret) return 1;
        return -1;
    }

    if (!data->was_removed && item != NULL)
        item->save_state = ISS_WAITING_FOR_SAVE;

    const char* item_name = Item::get_item_name(data->item_type);
    GS_ASSERT(item_name != NULL);
    if (item_name == NULL) return -1;

    const char* location_name = get_location_name(data->location_name_id);
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
        "HMSET data:%lld " // key
        "%s %lld "  // global id
        "%s %s "    // data name
        "%s %d "    // durability
        "%s %d "    // stack size
        "%s %s "    // location name   ("player:[hand,inventory,energy_tanks,etc} [container attached to agent]","container", "particle")
        "%s %d "    // location id     (meaning is depedent on location_name. for player it is player id. for container it is container id. for particle it is position (server id maybe later -- might be separate field))
        "%s %d",    // container slot (only relevant if container)
        data->global_id,
        ITEM_GUID_KEYNAME,           data->global_id,
        ITEM_NAME_KEYNAME,           item_name,
        ITEM_DURABILITY_KEYNAME,     data->durability,
        ITEM_STACK_SIZE_KEYNAME,     data->stack_size,
        ITEM_LOCATION_KEYNAME,       location_name,
        ITEM_LOCATION_ID_KEYNAME,    data->user_id,
        ITEM_CONTAINER_SLOT_KEYNAME, data->container_slot);
    CHECK_REDIS_OK(ret);

    ret = redisAsyncCommand(ctx, &player_item_add_cb, data,
        "SADD %s:%d %lld", location_name, data->user_id, data->global_id);
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
        data->global_id = item->global_id;
        data->item_type = item->type;
        data->durability = item->durability;
        data->stack_size = item->stack_size;
        data->location = item->location;
        data->location_id = item->location_id;
        data->container_slot = item->container_slot;
        data->location_name_id = location_name_id;
        data->was_removed = remove_items_after;
        
        int ret = save_player_item(data);
        GS_ASSERT(ret >= 0);
        if (ret < 0) printf("Error saving item: %lld:%d\n", item->global_id, item->id);
    }

    if (remove_items_after)
        for (int i=0; i<container->slot_max; i++)
            if (container->slot[i] != NULL_ITEM)
                Item::destroy_item(container->slot[i]);
}

void load_player_item(int64_t item_guid, class PlayerContainerLoadData* container_data)
{
    class PlayerItemLoadData* data = player_item_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return;

    data->item_guid = item_guid;
    data->container_data_id = container_data->id;

    int ret = redisAsyncCommand(ctx, &load_player_item_cb, data,
        "HGETALL item:%lld", item_guid);
    GS_ASSERT(ret == REDIS_OK);
} 

int begin_player_load(UserID user_id, int client_id)
{
    class PlayerLoadData* data = player_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return -1;
    data->user_id = user_id;
    data->client_id = client_id;

    int ret = redisAsyncCommand(ctx, &load_player_data_cb, data,
        "HGETALL player:%d", user_id);
    GS_ASSERT(ret == REDIS_OK);
    
    return data->id;
}

bool load_player_container(int player_load_id, int container_id)
{
    class PlayerLoadData* player_data = player_load_data_list->get(player_load_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL) return false;

    player_data->expect_container(container_id);

    NetPeerManager* client = NetServer::get_client(player_data->client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL) return false;
    GS_ASSERT(client->user_id != NULL_USER_ID);
    if (client->user_id == NULL_USER_ID) return false;

    ItemContainerType container_type = ItemContainer::get_container_type(container_id);
    LocationNameID loc_id = get_player_location_name_id(container_type);
    const char* location_name = get_location_name(loc_id);
    GS_ASSERT(location_name != NULL);
    if (location_name == NULL) return false;
    
    class PlayerContainerLoadData* data = player_container_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return false;
    data->container_id = container_id;
    data->player_data_id = player_data->id;

    int ret = redisAsyncCommand(ctx, &load_player_container_cb, data,
        "SMEMBERS %s:%d", location_name, client->user_id);
    GS_ASSERT(ret == REDIS_OK);    

    return (ret == REDIS_OK);
}

bool end_player_load(int player_load_id)
{
    class PlayerLoadData* player_data = player_load_data_list->get(player_load_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL) return false;

    player_data->setup_complete();
    return true;
}

}   // serializer
