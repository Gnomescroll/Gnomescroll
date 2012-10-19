#pragma once

namespace serializer
{

void init_players();
void teardown_players();

bool save_player(UserID user_id, AgentID agent_id);
bool save_player_container(ClientID client_id, int container_id);

int begin_player_load(UserID user_id, ClientID client_id, AgentID agent_id);
bool load_player_container(int player_load_id, int container_id); 
bool end_player_load(int player_load_id);

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

    PlayerLoadData(int id) :
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

    PlayerContainerLoadData(int id) :
        id(-1), container_id(NULL_CONTAINER), container_type(CONTAINER_TYPE_NONE),
        player_data_id(-1)
    {}
};

class PlayerLoadDataList: public ObjectList<class PlayerLoadData>
{
    public:
        const char* name() { return "PlayerLoadData"; }

    PlayerLoadDataList(unsigned int capacity) :
        ObjectList<class PlayerLoadData>(capacity)
    { this->print(); }
};

class PlayerContainerLoadDataList: public ObjectList<class PlayerContainerLoadData>
{
    public:
        const char* name() { return "PlayerContainerLoadData"; }

    PlayerContainerLoadDataList(unsigned int capacity) :
        ObjectList<class PlayerContainerLoadData>(capacity)
    { this->print(); }
};

}   // serializer
