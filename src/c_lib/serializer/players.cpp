#include "players.hpp"

#include <serializer/items.hpp>
#include <serializer/parse.hpp>
#include <serializer/_state.hpp>
#include <net_lib/common/net_peer_manager.hpp>

namespace serializer
{

static class PlayerLoadDataList* player_load_data_list = NULL;
static class PlayerContainerLoadDataList* player_container_load_data_list = NULL;

void init_players()
{
    GS_ASSERT(player_load_data_list == NULL);
    player_load_data_list = new class PlayerLoadDataList(PLAYER_LOAD_DATA_LIST_MAX);

    GS_ASSERT(player_container_load_data_list == NULL);
    player_container_load_data_list = new class PlayerContainerLoadDataList(PLAYER_CONTAINER_LOAD_DATA_LIST_MAX);
}

void teardown_players()
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
}

bool save_remote_player_data()
{ 
    for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
    {
        NetPeerManager* client = NetServer::clients[i];
        if (client == NULL) continue;
        if (!client->loaded) continue;
        GS_ASSERT(client->user_id != NULL_USER_ID && client->agent_id != NULL_AGENT);
        if (client->user_id == NULL_USER_ID || client->agent_id == NULL_AGENT) continue;
        save_player(client->user_id, client->agent_id);
        int n_containers = 0;
        int* containers = ItemContainer::get_player_containers(client->agent_id, &n_containers);
        GS_ASSERT(n_containers == N_PLAYER_CONTAINERS);
        for (int j=0; j<n_containers; j++)
            // TODO -- enable hand saving
            if (containers[j] != ItemContainer::get_agent_hand(client->agent_id))
                save_player_container(client->client_id, containers[j]);
    }
    
    return true;
}

bool PlayerLoadData::signal_if_loaded()
{
    GS_ASSERT(!this->waiting_for_setup);
    if (this->waiting_for_setup)
    {
        log_player_load_error("signal_if_loaded(): still waiting_for_setup", NULL,
            this, NULL, NULL, NULL, NULL);
        return false;
    }
    if (!this->player_data_loaded) return false;
    for (int i=0; i<this->n_containers_expected; i++)
        if (!this->containers_loaded[i])
        {
            log_player_load_error("signal_if_loaded(): not all containers actually loaded", NULL,
                this, NULL, NULL, NULL, NULL);
            return false;
        }

    NetPeerManager* client = NetServer::get_client(this->client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL)
    {
        log_player_load_error("signal_if_loaded(): NetPeerManager reference was NULL", NULL,
            this, NULL, NULL, NULL, NULL);
        return false;
    }
    if (client->user_id != this->user_id)
    {
        log_player_load_error("signal_if_loaded(): NetPeerManager user_id did not match player load data user_id", NULL,
            this, NULL, NULL, NULL, NULL);
        return false;
    }
    client->was_deserialized(&this->player_data);
    this->loaded = true;
    return true;
}

void PlayerLoadData::load_error()
{
    if (this->error) return;
    this->error = true;
    NetPeerManager* client = NetServer::get_client(this->client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL)
    {
        log_player_load_error("load_error(): NetPeerManager reference was NULL", NULL,
            this, NULL, NULL, NULL, NULL);
        return;
    }
    if (client->user_id != this->user_id)
    {
        log_player_load_error("load_error(): NetPeerManager user_id did not match player load data user_id", NULL,
            this, NULL, NULL, NULL, NULL);
        return;
    }
    client->deserializer_failed();
}


static bool parse_player_container_token(const char* key, const char* val, class ParsedPlayerContainerData* data)
{
    bool err = false;
    if (strcmp(NAME_TAG, key) == 0)
    {
        GS_ASSERT(ItemContainer::is_valid_container_name(val));
        if (!ItemContainer::is_valid_container_name(val)) return false;
        strncpy(data->name, val, LOCATION_NAME_MAX_LENGTH);
        data->name[LOCATION_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(USER_ID_TAG, key) == 0)
    {
        long long user_id = parse_int(val, err);
        GS_ASSERT(!err && user_id > 0 && user_id < INT32_MAX);
        if (err || user_id <= 0 || user_id > INT32_MAX) return false;
        data->user_id = (UserID)user_id;
    }
    else
    if (strcmp(CONTAINER_ITEM_COUNT_TAG, key) == 0)
    {
        long long container_count = parse_int(val, err);
        GS_ASSERT(!err && container_count >= 0 && container_count < MAX_CONTAINER_SIZE);
        if (err || container_count < 0 || container_count > MAX_CONTAINER_SIZE) return false;
        data->container_count = (int)container_count;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool parse_player_token(const char* key, const char* val, class ParsedPlayerData* data)
{
    bool err = false;
    if (strcmp(COLOR_TAG, key) == 0)
    {
        static char buf[COLOR_LENGTH+1] = {'\0'};
        strncpy(buf, val, COLOR_LENGTH);
        buf[COLOR_LENGTH] = '\0';
        
        int pts = 1;
        char d;
        int j = 0;
        int cmp_len = 0;
        while ((d = buf[j]) != '\0')
        {
            if (d == COLOR_COMPONENT_DELIMITER[0])
            {
                GS_ASSERT(cmp_len == COLOR_COMPONENT_LENGTH);
                if (cmp_len != COLOR_COMPONENT_LENGTH) return false;
                cmp_len = 0;
                pts++;
                buf[j] = '\0';
            }
            else
                cmp_len++;
            j++;
        }
        GS_ASSERT(cmp_len == COLOR_COMPONENT_LENGTH);
        if (cmp_len != COLOR_COMPONENT_LENGTH) return false;
        GS_ASSERT(pts == 3);
        if (pts != 3) return false;

        int base_offset = COLOR_COMPONENT_LENGTH + COLOR_COMPONENT_DELIMITER_LENGTH;
        long long r = parse_int(&buf[0 * base_offset], err);
        GS_ASSERT(!err && r >= 0 && r <= 255);
        if (err || r < 0 || r > 255) return false;
        long long g = parse_int(&buf[1 * base_offset], err);
        GS_ASSERT(!err && g >= 0 && g <= 255);
        if (err || g < 0 || g > 255) return false;
        long long b = parse_int(&buf[2 * base_offset], err);
        GS_ASSERT(!err && b >= 0 && b <= 255);
        if (err || b < 0 || b > 255) return false;

        data->color.r = (unsigned char)r;
        data->color.g = (unsigned char)g;
        data->color.b = (unsigned char)b;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

bool process_player_container_blob(const char* str, class PlayerLoadData* player_load_data, class PlayerContainerLoadData* container_load_data)
{    
    // allocate scratch buffer long enough to hold the largest line
    static const size_t LONGEST_LINE = GS_MAX(ITEM_LINE_LENGTH, PLAYER_CONTAINER_LINE_LENGTH);
    char buf[LONGEST_LINE+1] = {'\0'};
    char item_buf[ITEM_LINE_LENGTH+1] = {'\0'};

    ItemLocationType location = IL_NOWHERE;
    if (container_load_data->container_type == AGENT_HAND)
        location = IL_HAND;
    else
        location = IL_CONTAINER;

    // copy container header line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i] = '\0';
    GS_ASSERT(c == '\n' || c == '\0');
    if (c != '\0' && c != '\n')
    {
        log_player_load_error("Player container string malformed", buf,
            player_load_data, container_load_data, NULL, NULL, NULL);
        return false;
    }
    
    // read header
    class ParsedPlayerContainerData container_data;
    parse_line<class ParsedPlayerContainerData>(&parse_player_container_token, buf, i, &container_data);
    GS_ASSERT(container_data.valid);
    if (!container_data.valid)
    {
        log_player_load_error("Parsed player container data invalid", buf,
            player_load_data, container_load_data, NULL, &container_data, NULL);
        return false;
    }
    i++;

    // Check properties on the container data
    // The parser only makes sure the values it found were in range
    // It doesn't handle missing values (so that the format can remain flexible)
    
    GS_ASSERT(container_data.user_id == player_load_data->user_id);
    if (container_data.user_id != player_load_data->user_id)
    {
        log_player_load_error("Player load data user_id does not match container_data user_id (bug)", buf,
            player_load_data, container_load_data, NULL, &container_data, NULL);
        return false;
    }

    // make sure location name matches container type
    const char* expected_container_name = ItemContainer::get_container_name(container_load_data->container_type);
    GS_ASSERT(expected_container_name != NULL && strcmp(expected_container_name, container_data.name) == 0);
    if (expected_container_name == NULL || strcmp(expected_container_name, container_data.name) != 0)
    {
        log_player_load_error("Player container name does not match expected container name", buf,
            player_load_data, container_load_data, NULL, &container_data, NULL);
        return false;
    }

    // make sure container count is less than the container's size
    unsigned int max_slots = ItemContainer::get_container_max_slots(container_load_data->container_type);
    GS_ASSERT(container_data.container_count <= max_slots);
    if (container_data.container_count > max_slots)
    {
        log_player_load_error("Player container count exceeds container max slots", buf,
            player_load_data, container_load_data, NULL, &container_data, NULL);
        return false;
    }

    bool error = false;
    clear_slot_checker();

    // parse items
    for (unsigned int j=0; j<container_data.container_count; j++)
    {
        GS_ASSERT(c != '\0');
        if (c == '\0') break;

        size_t k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < ITEM_LINE_LENGTH)
        {
            item_buf[k] = c;
            buf[k] = c;
            k++;
        }
        item_buf[k] = '\0';
        buf[k] = '\0';
        GS_ASSERT(c == '\n');
        if (c != '\n')
        {
            error = true;
            log_player_load_error("Player item string is malformed", buf,
                player_load_data, container_load_data, NULL, &container_data, NULL);
            break;
        }

        class ParsedItemData* item_data = create_item_data();
        GS_ASSERT(item_data != NULL);
        if (item_data == NULL)
        {
            error = true;
            log_player_load_error("Failed to create item", buf,
                player_load_data, container_load_data, NULL, &container_data, item_data);
            break;
        }

        parse_line<class ParsedItemData>(&parse_item_token, item_buf, k, item_data);
        const char* actual_name = Item::get_compatible_item_name(item_data->name);

        int item_type = NULL_ITEM_TYPE;
        if (actual_name != NULL)
            item_type = Item::get_item_type(actual_name);
        
        GS_ASSERT(item_data->valid);
        GS_ASSERT(actual_name != NULL);
        GS_ASSERT(item_type != NULL_ITEM_TYPE);
        GS_ASSERT(item_data->container_slot < max_slots);
        bool slot_occupied = false;
        if (item_data->container_slot < max_slots)
        {
            slot_occupied = (slot_checker[item_data->container_slot] != NULL_ITEM);
            GS_ASSERT(!slot_occupied);
            if (slot_occupied)
                printf("Slot %d is occupied with item %d\n", item_data->container_slot, slot_checker[item_data->container_slot]);
        }

        if (!item_data->valid || item_data->container_slot >= max_slots
          || actual_name == NULL || item_type == NULL_ITEM_TYPE || slot_occupied)
        {
            error = true;
            destroy_item_data(item_data->id);
            log_player_load_error("Player container contains invalid item", buf,
                player_load_data, container_load_data, NULL, &container_data, item_data);
            continue;
        }

        // attach post-process data needed for the final item creation
        item_data->item_type = item_type;
        item_data->item_location = location;
        item_data->item_container_type = container_load_data->container_type;

        // put item in the slot checker
        slot_checker[item_data->container_slot] = item_data->id;
    }

    if (error) clear_item_data();

    return (!error);
}

bool process_player_blob(const char* str, class PlayerLoadData* player_load_data)
{
    static const size_t LONGEST_LINE = PLAYER_LINE_LENGTH;
    char buf[LONGEST_LINE+1] = {'\0'};

    // copy main data line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i] = '\0';
    GS_ASSERT(c == '\0');
    if (c != '\0')
    {
        log_player_load_error("Player save data is malformed: (next line)", buf,
            player_load_data, NULL, NULL, NULL, NULL);
        return false;
    }

    class ParsedPlayerData player_data;
    parse_line<class ParsedPlayerData>(&parse_player_token, buf, i, &player_load_data->player_data);
    GS_ASSERT(player_load_data->player_data.valid);
    if (!player_load_data->player_data.valid)
    {
        log_player_load_error("ParsedPlayerData is invalid.", buf, player_load_data, NULL,
            &player_load_data->player_data, NULL, NULL);
        return false;
    }
    i++;
    
    return true;
}

void player_load_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    received_redis_reply();
    
    class PlayerLoadData* data = (class PlayerLoadData*)_data;
    redisReply* reply = (redisReply*)_reply;

    bool worked = true;
    if (reply->type == REDIS_REPLY_STRING)
    {
        worked = process_player_blob(reply->str, data);
        if (worked)
            data->player_data_was_loaded();
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
        data->player_data_was_loaded();
    else
    {
        GS_ASSERT(false);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            log_player_load_error("Player load redis callback returned error", reply->str,
                data, NULL, NULL, NULL, NULL);
        }
        else
        {
            log_player_load_error("Player load redis callback returned unrecognized reply type", NULL,
                data, NULL, NULL, NULL, NULL);
        }
        worked = false;
    }

    if (!worked) data->load_error();
    
    player_load_data_list->destroy(data->id);
}

void player_container_load_cb(redisAsyncContext* ctx, void* _reply, void* _data)
{
    received_redis_reply();

    redisReply* reply = (redisReply*)_reply;
    class PlayerContainerLoadData* data = (class PlayerContainerLoadData*)_data;
    class PlayerLoadData* player_data = player_load_data_list->get(data->player_data_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL)
    {
        log_player_load_error("PlayerLoadData not found from PlayerContainerLoadData player_data_id", NULL,
            NULL, data, NULL, NULL, NULL);
        return;
    }

    bool worked = true;
    if (reply->type == REDIS_REPLY_STRING)
    {
        worked = process_player_container_blob(reply->str, player_data, data);
        GS_ASSERT(worked);
        if (worked)
            player_data->container_was_loaded(data->container_type);
    }
    else
    if (reply->type == REDIS_REPLY_NIL)
        player_data->container_was_loaded(data->container_type);
    else
    {
        GS_ASSERT(false);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            log_player_load_error("Player container load redis callback returned error", reply->str,
                player_data, data, NULL, NULL, NULL);
        }
        else
        {
            log_player_load_error("Player container load redis callback returned unrecognized reply type", NULL,
                player_data, data, NULL, NULL, NULL);
        }
        worked = false;
    }

    if (!worked)
    {
        clear_item_data();
        player_data->load_error();
    }

    player_container_load_data_list->destroy(data->id);
}

const char* write_player_container_string(int container_id, UserID user_id)
{
    class ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL)
    {
        log_player_save_error("Could not save container; container was NULL", user_id, NULL_AGENT, NULL);
        return NULL;
    }

    const char* container_name = ItemContainer::get_container_name(container->type);
    GS_ASSERT(container_name != NULL);
    if (container_name == NULL)
    {
        log_player_save_error("Could not save container; container_name was NULL", user_id, NULL_AGENT, container);
        return NULL;
    }

    size_t ibuf = 0;
    
    // write header
    int could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, PLAYER_CONTAINER_HEADER_FMT,
        container_name, user_id, container->slot_count);
    GS_ASSERT(could_write > 0 && (size_t)could_write < BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf)
    {
        log_player_save_error("Could not save container; overran buffer", user_id, NULL_AGENT, container);
        return NULL;
    }
    ibuf += (size_t)could_write;
    
    _buffer[ibuf++] = '\n';
    if (ibuf >= BUF_SIZE)
    {
        log_player_save_error("Could not save container; overran buffer", user_id, NULL_AGENT, container);
        return NULL;
    }

    if (container->slot_count > 0)
    {   // we must check the slot count, because write_container_contents_string returns 0 on error or if the container is empty
        size_t wrote = write_container_contents_string(&_buffer[ibuf], BUF_SIZE - ibuf, container);
        if (wrote <= 0)
        {
            log_player_save_error("Could not save container; failed to write contents to buffer", user_id, NULL_AGENT, container);
            return NULL;
        }
        ibuf += wrote;
    }
    
    _buffer[ibuf] = '\0';

    return _buffer;
}

const char* write_player_string(AgentID agent_id)
{
    class Agent* agent = Agents::get_agent(agent_id);
    GS_ASSERT(agent != NULL);
    if (agent == NULL)
    {
        log_player_save_error("Could not save player string; agent not found", NULL_USER_ID, agent_id, NULL);
        return NULL;
    }

    size_t ibuf = 0;

    int could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, PLAYER_DATA_FMT,
        agent->status.color.r, agent->status.color.g, agent->status.color.b);
        
    GS_ASSERT(could_write > 0 && (size_t)could_write < BUF_SIZE - ibuf);
    if (could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf)
    {
        log_player_save_error("Could not save player string; overran buffer", NULL_USER_ID, agent_id, NULL);
        return NULL;
    }
    ibuf += could_write;

    return _buffer;
}

bool save_player_container(ClientID client_id, int container_id)
{
    if (!Options::serializer || !Options::auth) return true;

    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return false;
    
    NetPeerManager* client = NetServer::get_client(client_id);
    GS_ASSERT(client != NULL);
    if (client == NULL) return false;
    GS_ASSERT(client->user_id != NULL_USER_ID);
    if (client->user_id == NULL_USER_ID) return false;

    ItemContainerType container_type = ItemContainer::get_container_type(container_id);
    const char* location_name = get_player_container_location_name(container_type);
    GS_ASSERT(location_name != NULL);
    if (location_name == NULL) return false;

    const char* container_string = write_player_container_string(container_id, client->user_id);
    GS_ASSERT(container_string != NULL);
    if (container_string == NULL) return false;

    int ret = send_redis_command(ctx, NULL, NULL,
        "SET %s:%d %s", location_name, client->user_id, container_string);

    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

bool save_player(UserID user_id, AgentID agent_id)
{
    if (!Options::serializer || !Options::auth) return true;
    
    ASSERT_VALID_USER_ID(user_id);
    IF_INVALID_USER_ID(user_id) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    
    const char* player_string = write_player_string(agent_id);
    GS_ASSERT(player_string != NULL);
    if (player_string == NULL) return false;
    
    int ret = send_redis_command(ctx, NULL, NULL,
        "SET " PLAYER_REDIS_KEY_PREFIX "%d %s", user_id, player_string);

    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

int begin_player_load(UserID user_id, ClientID client_id)
{
    if (!Options::serializer || !Options::auth) return true;

    ASSERT_VALID_USER_ID(user_id);
    IF_INVALID_USER_ID(user_id) return -1;
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return -1;

    class PlayerLoadData* data = player_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return -1;
    
    data->setup_begin();
    data->user_id = user_id;
    data->client_id = client_id;
        
    return data->id;
}

bool load_player_container(int player_load_id, ItemContainerType container_type)
{
    if (!Options::serializer || !Options::auth) return true;

    GS_ASSERT(container_type != AGENT_HAND);

    class PlayerLoadData* player_data = player_load_data_list->get(player_load_id);
    GS_ASSERT(player_data != NULL);
    if (player_data == NULL) return false;
    if (!player_data->expect_container(container_type)) return false;

    class PlayerContainerLoadData* data = player_container_load_data_list->create();
    GS_ASSERT(data != NULL);
    if (data == NULL) return false;

    const char* container_name = get_player_container_location_name(container_type);
    GS_ASSERT(container_name != NULL);
    if (container_name == NULL) return false;

    data->container_type = container_type;
    data->player_data_id = player_data->id;

    int ret = send_redis_command(ctx, &player_container_load_cb, data,
        "GET %s:%d", container_name, player_data->user_id);
        
    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

bool end_player_load(int player_load_id)
{
    if (!Options::serializer || !Options::auth) return true;

    class PlayerLoadData* data = player_load_data_list->get(player_load_id);
    GS_ASSERT(data != NULL);
    if (data == NULL) return false;

    GS_ASSERT(data->user_id != NULL_USER_ID);
    if (data->user_id == NULL_USER_ID) return false;

    data->setup_complete();

    // Call the player data load last, so we can destroy the player load data struct safely in its callback
    // callbacks return in-order
    int ret = send_redis_command(ctx, &player_load_cb, data,
        "GET " PLAYER_REDIS_KEY_PREFIX "%d", data->user_id);
    
    GS_ASSERT(ret == REDIS_OK);
    return (ret == REDIS_OK);
}

// Called from outside this module. NetPeerManager is the only thing that knows about true failure
void player_load_failed()
{
    if (!Options::serializer || !Options::auth) return;
    clear_item_data();
}

}   // serializer
