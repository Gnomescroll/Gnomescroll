#include "items.hpp"

#include <item/item.hpp>
#include <item/common/constants.hpp>
#include <serializer/redis.hpp>

namespace serializer
{

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
        this->loaded = true;
        return true;
    }
        
    public:
        int id;
        UserID user_id;
        AgentID agent_id;
        ClientID client_id;

        bool waiting_for_setup; // this object is setup in multiple steps, we need to indicate when we're done

        // loaded state
        bool player_data_loaded;

        int n_containers_expected;
        int containers_expected[N_PLAYER_CONTAINERS];
        bool containers_loaded[N_PLAYER_CONTAINERS];

        bool loaded;    // completely

    void setup_begin()
    {
        GS_ASSERT(this->waiting_for_setup);
        this->waiting_for_setup = true;
    }

    void setup_complete()
    {
        GS_ASSERT(this->waiting_for_setup);
        this->waiting_for_setup = false;
    }

    bool expect_container(int container_id)
    {
        GS_ASSERT(this->n_containers_expected < N_PLAYER_CONTAINERS);
        if (this->n_containers_expected >= N_PLAYER_CONTAINERS) return false;
        this->containers_expected[this->n_containers_expected++] = container_id;
        return true;
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
        id(-1), user_id(NULL_USER_ID), agent_id(NULL_AGENT), client_id(NULL_CLIENT),
        waiting_for_setup(true),
        player_data_loaded(false), n_containers_expected(0),
        loaded(false)
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
        ItemContainerType container_type;
        int player_data_id;

    PlayerContainerLoadData() :
        id(-1), container_id(NULL_CONTAINER), container_type(CONTAINER_TYPE_NONE),
        player_data_id(-1)
    {}
};

class PlayerLoadDataList: public ElasticObjectList<class PlayerLoadData, PLAYER_LOAD_DATA_LIST_INITIAL_SIZE>
{
    public:
        const char* name() { return "PlayerLoadData"; }

    PlayerLoadDataList() :
        ElasticObjectList<class PlayerLoadData, PLAYER_LOAD_DATA_LIST_INITIAL_SIZE>(PLAYER_LOAD_DATA_LIST_HARD_MAX)
    { this->print(); }
};

class PlayerContainerLoadDataList: public ElasticObjectList<class PlayerContainerLoadData, PLAYER_CONTAINER_LOAD_DATA_LIST_INITIAL_SIZE>
{
    public:
        const char* name() { return "PlayerContainerLoadData"; }

    PlayerContainerLoadDataList() :
        ElasticObjectList<class PlayerContainerLoadData, PLAYER_CONTAINER_LOAD_DATA_LIST_INITIAL_SIZE>(PLAYER_CONTAINER_LOAD_DATA_LIST_HARD_MAX)
    { this->print(); }
};

static class PlayerLoadDataList* player_load_data_list = NULL;
static class PlayerContainerLoadDataList* player_container_load_data_list = NULL;

static const size_t SRL_BUF_SIZE = 0xffff-1;
static char* _srl_buf = NULL;

void init_items()
{
    GS_ASSERT(player_load_data_list == NULL);
    player_load_data_list = new class PlayerLoadDataList;

    GS_ASSERT(player_container_load_data_list == NULL);
    player_container_load_data_list = new class PlayerContainerLoadDataList;

    GS_ASSERT(_srl_buf == NULL);
    _srl_buf = (char*)calloc(SRL_BUF_SIZE+1, sizeof(char));
}

void teardown_items()
{
    if (player_load_data_list != NULL)
    {
        GS_ASSERT(player_load_data_list->ct == 0);
        delete player_load_data_list;
    }
    if (player_container_load_data_list != NULL)
    {
        GS_ASSERT(player_container_load_data_list->ct == 0);
        delete player_container_load_data_list;
    }
    if (_srl_buf != NULL) free(_srl_buf);
}

class ParsedItemData
{
    public:
        int32_t global_id;
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
        this->global_id = 0;
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

class ParsedPlayerContainerData
{
    public:
        char name[LOCATION_NAME_MAX_LENGTH+1];
        UserID user_id;
        unsigned int container_count;
        bool valid;

    void reset()
    {
        this->user_id = NULL_USER_ID;
        this->container_count = -1;
        memset(this->name, 0, sizeof(this->name));
        this->valid = false;
    }

    ParsedPlayerContainerData()
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
        if (strncmp(GLOBAL_ID_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long global_id = parse_int(val, err);
            GS_ASSERT(!err);
            if (err) return;
            //if (global_id <= 0 || global_id > INT32_MAX) return;  // TODO -- re-enable once global id acquisition in place
            data->global_id = (int32_t)global_id;
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

// WARNING: modifies char* str
void parse_player_container_header(char* str, const size_t length, class ParsedPlayerContainerData* data)
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
        if (strncmp(NAME_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            GS_ASSERT(ItemContainer::is_valid_container_name(val));
            if (!ItemContainer::is_valid_container_name(val)) return;
            strncpy(data->name, val, CONTAINER_NAME_MAX_LENGTH);
            data->name[CONTAINER_NAME_MAX_LENGTH] = '\0';
        }
        else
        if (strncmp(USER_ID_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long user_id = parse_int(val, err);
            GS_ASSERT(!err && user_id > 0 && user_id < INT32_MAX);
            if (err || user_id <= 0 || user_id > INT32_MAX) return;
            data->user_id = (UserID)user_id;
        }
        else
        if (strncmp(CONTAINER_COUNT_TAG TAG_DELIMITER, key, TAG_LENGTH + TAG_DELIMITER_LENGTH) == 0)
        {
            long long container_count = parse_int(val, err);
            GS_ASSERT(!err && container_count >= 0 && container_count < MAX_CONTAINER_SIZE);
            if (err || container_count < 0 || container_count > MAX_CONTAINER_SIZE) return;
            data->container_count = (int)container_count;
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

void process_player_container_blob(const char* str, class PlayerLoadData* player_load_data, class PlayerContainerLoadData* container_load_data)
{    
    // allocate scratch buffer long enough to hold the largest line
    static const size_t LONGEST_LINE  = GS_MAX(ITEM_LINE_LENGTH, PLAYER_CONTAINER_LINE_LENGTH);
    static char buf[LONGEST_LINE+1];
    if (strnlen(str, LONGEST_LINE) < LONGEST_LINE) return;    // TODO -- error handling 
    buf[LONGEST_LINE] = '\0';

    ItemLocationType location = IL_NOWHERE;
    int location_id = NULL_LOCATION;
    if (container_load_data->container_type == AGENT_HAND)
    {
        location = IL_HAND;
        location_id = player_load_data->agent_id;
    }
    else
    {
        location = IL_CONTAINER;
        location_id = container_load_data->container_id;
    }

    // copy container header line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i++] = '\0';
    
    // read header
    class ParsedPlayerContainerData container_data;
    parse_player_container_header(buf, i, &container_data);
    GS_ASSERT(container_data.valid);
    if (!container_data.valid) return;  // TODO -- log error

    // Check properties on the container data
    // The parser only makes sure the values it found were in range
    // It doesn't handle missing values (so that the format can remain flexible)
    
    GS_ASSERT(container_data.user_id == player_load_data->user_id);
    if (container_data.user_id != player_load_data->user_id) return;  // TODO -- log error

    // make sure location name matches container type
    const char* expected_container_name = ItemContainer::get_container_name(container_load_data->container_type);
    GS_ASSERT(expected_container_name != NULL && strcmp(expected_container_name, container_data.name) == 0);
    if (expected_container_name == NULL || strcmp(expected_container_name, container_data.name) != 0) return;   // TODO -- log error

    // make sure container count is less than the container's size
    unsigned int max_slots = ItemContainer::get_container_max_slots(container_load_data->container_type);
    GS_ASSERT(container_data.container_count <= max_slots);
    if (container_data.container_count > max_slots) return; // TODO -- log error

    // parse items
    class ParsedItemData item_data;
    for (unsigned int j=0; j<container_data.container_count; j++)
    {
        GS_ASSERT(c != '\0');
        if (c == '\0') break;

        size_t k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
        buf[k] = '\0';

        item_data.reset();
        parse_item_string(buf, k, &item_data);
        GS_ASSERT(item_data.valid);
        if (!item_data.valid) continue; // TODO -- log error

        // TODO - Apply renaming scheme to get item type
        int item_type = Item::get_item_type(item_data.name);
        GS_ASSERT(item_type != NULL_ITEM_TYPE);
        if (item_type == NULL_ITEM_TYPE) continue;  // TODO -- log error

        GS_ASSERT(item_data.container_slot < max_slots);
        if (item_data.container_slot >= max_slots) continue;    // TODO -- log error

        // create item
        class Item::Item* item = Item::create_item_for_loading();
        GS_ASSERT(item != NULL);
        if (item == NULL) continue; // TODO -- log error. RESERVE ITEM SPACE FOR LOADING

        item->global_id = item_data.global_id;
        item->type = item_type;
        item->durability = item_data.durability;
        item->stack_size = item_data.stack_size;
        item->container_slot = item_data.container_slot;
        item->location = location;
        item->location_id = location_id;

        item->init_for_loading();        

        // TODO -- catch duplicate inserts into container as error
        if (location == IL_HAND)
            ItemContainer::load_item_into_hand(item->id, player_load_data->agent_id);
        else
        if (location == IL_CONTAINER)
            ItemContainer::load_item_into_container(item->id, container_load_data->container_id, item->container_slot);            
    }
}

void player_load_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    class PlayerLoadData* data = (class PlayerLoadData*)_data;
    redisReply* reply = (redisReply*)_reply;

    if (reply->type == REDIS_REPLY_STRING)
    {
        // TODO -- catch errors and abort -- yes, players inventory will get corrupted otherwise
        data->player_data_was_loaded();
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
        data->player_data_was_loaded();
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(reply->type != REDIS_REPLY_ERROR);
        printf("Loading player %d failed with redis error: %s\n", data->user_id, reply->str); 
    }
    
    player_load_data_list->destroy(data->id);
}

void player_container_load_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    redisReply* reply = (redisReply*)_reply;
    class PlayerContainerLoadData* data = (class PlayerContainerLoadData*)_data;
    class PlayerLoadData* player_data = player_load_data_list->get(data->player_data_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL) return;    // TODO -- better handling
    
    if (reply->type == REDIS_REPLY_STRING)
    {
        process_player_container_blob(reply->str, player_data, data);
        // TODO -- catch errors and abort -- yes, players inventory will get corrupted otherwise
        player_data->container_was_loaded(data->container_id);
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
        player_data->container_was_loaded(data->container_id);
    else
    if (reply->type == REDIS_REPLY_ERROR)
    {
        GS_ASSERT(reply->type != REDIS_REPLY_ERROR);
        printf("Loading player %d container %d failed with redis error: %s\n", player_data->user_id, data->container_id, reply->str); 
    }

    player_container_load_data_list->destroy(data->id);
}

size_t write_item_string(char* buf, size_t buffer_size, ItemID item_id)
{
    // TODO -- check/acquire global id
    // just get a global id block from redis periodically

    // TODO -- validate item

    class Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return 0;

    const char* item_name = Item::get_item_name(item->type);
    GS_ASSERT(item_name != NULL);
    if (item_name == NULL) return 0;

    int could_write = snprintf(buf, buffer_size, ITEM_FMT,
        item->global_id,
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

const char* write_player_container_string(int container_id, UserID user_id)
{
    class ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return NULL;

    const char* container_name = ItemContainer::get_container_name(container->type);
    GS_ASSERT(container_name != NULL);
    if (container_name == NULL) return NULL;

    size_t ibuf = 0;
    
    // write header
    int could_write = snprintf(&_srl_buf[ibuf], SRL_BUF_SIZE - ibuf, PLAYER_CONTAINER_HEADER_FMT,
        container_name, user_id, container->slot_count);
    GS_ASSERT(could_write > 0 && (size_t)could_write < SRL_BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= SRL_BUF_SIZE - ibuf) return NULL;
    ibuf += (size_t)could_write;
    
    _srl_buf[ibuf++] = '\n';
    if (ibuf >= SRL_BUF_SIZE) return NULL;

    if (container->slot_count > 0)
    {   // we must check the slot count, because write_container_contents_string returns 0 on error or if the container is empty
        size_t wrote = write_container_contents_string(&_srl_buf[ibuf], SRL_BUF_SIZE - ibuf, container);
        if (wrote <= 0) return NULL;  // error
        ibuf += wrote;
    }
    
    _srl_buf[ibuf] = '\0';

    return _srl_buf;
}

const char* write_player_string(AgentID agent_id)
{
    class Agent_state* agent = ServerState::agent_list->get(agent_id);
    GS_ASSERT(agent != NULL);
    if (agent == NULL) return NULL;

    size_t ibuf = 0;

    int could_write = snprintf(&_srl_buf[ibuf], SRL_BUF_SIZE - ibuf, PLAYER_DATA_FMT,
        agent->status.color.r, agent->status.color.g, agent->status.color.b,
        agent->status.spawner);
    GS_ASSERT(could_write > 0 && (size_t)could_write < SRL_BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= SRL_BUF_SIZE - ibuf) return NULL;
    ibuf += could_write;

    return _srl_buf;
    
}

bool save_player_container(ClientID client_id, int container_id)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return false;
    
    NetPeerManager* client = NetServer::get_client(client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL) return false;
    GS_ASSERT(client->user_id != NULL_USER_ID);
    if (client->user_id == NULL_USER_ID) return false;

    ItemContainerType container_type = ItemContainer::get_container_type(container_id);
    LocationNameID loc_id = get_container_location_name_id(container_type);
    const char* location_name = get_location_name(loc_id);
    GS_ASSERT(location_name != NULL);
    if (location_name == NULL) return false;

    const char* container_string = write_player_container_string(container_id, client->user_id);
    GS_ASSERT(container_string != NULL);
    if (container_string == NULL) return false;

    int ret = redisAsyncCommand(ctx, NULL, NULL,
        "SET %s:%d %s", location_name, client->user_id, container_string);

    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

bool save_player(UserID user_id, AgentID agent_id)
{
    ASSERT_VALID_USER_ID(user_id);
    IF_INVALID_USER_ID(user_id) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    
    const char* player_string = write_player_string(agent_id);
    GS_ASSERT(player_string != NULL);
    if (player_string == NULL) return false;
    
    int ret = redisAsyncCommand(ctx, NULL, NULL,
        "SET " PLAYER_REDIS_KEY_PREFIX "%d %s", user_id, player_string);

    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

int begin_player_load(UserID user_id, ClientID client_id, AgentID agent_id)
{
    ASSERT_VALID_USER_ID(user_id);
    IF_INVALID_USER_ID(user_id) return -1;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return -1;
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return -1;

    class PlayerLoadData* data = player_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return -1;
    
    data->setup_begin();
    data->user_id = user_id;
    data->agent_id = agent_id;
    data->client_id = client_id;
        
    return data->id;
}

bool load_player_container(int player_load_id, int container_id)
{
    class PlayerLoadData* player_data = player_load_data_list->get(player_load_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL) return false;
    if (!player_data->expect_container(container_id)) return false;

    class PlayerContainerLoadData* data = player_container_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return false;

    ItemContainerType container_type = ItemContainer::get_container_type(container_id);
    GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
    if (container_type == CONTAINER_TYPE_NONE) return false;
    const char* container_name = get_location_name(get_container_location_name_id(container_type));
    GS_ASSERT(container_name != NULL);
    if (container_name == NULL) return false;

    data->container_id = container_id;
    data->container_type = container_type;
    data->player_data_id = player_data->id;

    int ret = redisAsyncCommand(ctx, &player_container_load_cb, data,
        "GET %s:%d", container_name, player_data->user_id);
        
    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

bool end_player_load(int player_load_id)
{
    class PlayerLoadData* data = player_load_data_list->get(player_load_id);
    GS_ASSERT(data != NULL);
    if (data == NULL) return false;

    GS_ASSERT(data->user_id != NULL_USER_ID);
    if (data->user_id == NULL_USER_ID) return false;

    data->setup_complete();

    // Call the player data load last, so we can destroy the player load data struct safely in its callback
    // callbacks return in-order
    int ret = redisAsyncCommand(ctx, &player_load_cb, data,
        "GET " PLAYER_REDIS_KEY_PREFIX "%d", data->user_id);
    
    GS_ASSERT(ret == REDIS_OK);
    return (ret != REDIS_OK);
}

}   // serializer
