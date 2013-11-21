/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/template/object_list.hpp>
#include <agent/constants.hpp>
#include <item/common/constants.hpp>
#include <serializer/constants.hpp>

namespace serializer
{

void init_players();
void teardown_players();

bool save_remote_player_data();

bool save_player(UserID user_id, AgentID agent_id);
bool save_player_container(ClientID client_id, ItemContainerID container_id);

int begin_player_load(UserID user_id, ClientID client_id);
bool load_player_container(int player_load_id, ItemContainerType container_type);
bool end_player_load(int player_load_id);

// Callback into this module. NetPeerManager is the only thing that knows about actual failure
// this module needs to know if the process failed outside of here so it can clean up correctly
void player_load_failed();

class ParsedPlayerData
{
    public:
        Color color;
        bool valid;

    void reset()
    {
        this->color = AGENT_DEFAULT_COLOR;
        this->valid = false;
    }

    ParsedPlayerData()
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

class PlayerLoadData
{
    private:
        bool signal_if_loaded();

    public:
        int id;
        UserID user_id;
        ClientID client_id;

        bool waiting_for_setup; // this object is setup in multiple steps, we need to indicate when we're done

        // loaded state
        bool player_data_loaded;

        size_t n_containers_expected;
        ItemContainerType containers_expected[N_PLAYER_CONTAINERS];
        bool containers_loaded[N_PLAYER_CONTAINERS];

        bool loaded;    // completely
        bool error;

        class ParsedPlayerData player_data;

    void load_error();

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

    bool expect_container(ItemContainerType container_type)
    {
        IF_ASSERT(container_type == NULL_CONTAINER_TYPE) return false;
        IF_ASSERT(this->n_containers_expected >= N_PLAYER_CONTAINERS) return false;
        for (size_t i=0; i<this->n_containers_expected; i++)
        {
            IF_ASSERT(this->containers_expected[i] == container_type) return false;
        }
        this->containers_expected[this->n_containers_expected++] = container_type;
        return true;
        printf("Expecting container %d. %lu expected total\n", container_type, (unsigned long)this->n_containers_expected);
    }

    bool container_was_loaded(ItemContainerType container_type)
    {
        if (this->error) return false;
        for (size_t i=0; i<this->n_containers_expected; i++)
            if (this->containers_expected[i] == container_type)
            {
                GS_ASSERT(!this->containers_loaded[i]);
                this->containers_loaded[i] = true;
                return this->signal_if_loaded();
            }
        return false;
    }

    bool player_data_was_loaded()
    {
        if (this->error) return false;
        GS_ASSERT(!this->player_data_loaded);
        this->player_data_loaded = true;
        return this->signal_if_loaded();
    }

    explicit PlayerLoadData(int id) :
        id(-1), user_id(NULL_USER_ID), client_id(NULL_CLIENT),
        waiting_for_setup(true),
        player_data_loaded(false), n_containers_expected(0),
        loaded(false), error(false)
    {
        for (size_t i=0; i<N_PLAYER_CONTAINERS; i++)
        {
            this->containers_expected[i] = NULL_CONTAINER_TYPE;
            this->containers_loaded[i] = false;
        }
    }
};

class PlayerContainerLoadData
{
    public:
        int id;
        ItemContainerType container_type;
        int player_data_id;

    explicit PlayerContainerLoadData(int id) :
        id(-1), container_type(NULL_CONTAINER_TYPE), player_data_id(-1)
    {}
};

class PlayerLoadDataList: public ObjectList<class PlayerLoadData>
{
    public:
        const char* name() { return "PlayerLoadData"; }

    explicit PlayerLoadDataList(size_t capacity) :
        ObjectList<class PlayerLoadData>(capacity)
    {}
};

class PlayerContainerLoadDataList: public ObjectList<class PlayerContainerLoadData>
{
    public:
        const char* name() { return "PlayerContainerLoadData"; }

    explicit PlayerContainerLoadDataList(size_t capacity) :
        ObjectList<class PlayerContainerLoadData>(capacity)
    {}
};

}   // serializer
