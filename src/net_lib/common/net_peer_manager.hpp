#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <agent/constants.hpp>
#include <item/common/constants.hpp>

class NetPeerManager
{
    private:
        int get_container_loaded_index_for_type(ItemContainerType type);
        void was_deserialized();    // this will be called internally once everything expected has been loaded

    public:
        ClientID client_id;
        AgentID agent_id;
        bool inited;
        bool loaded;
        bool waiting_for_auth;
        bool authorized;

        // serialization init state
        bool deserialized;
        bool player_data_loaded;
        bool containers_loaded[Item::N_PLAYER_CONTAINERS];
        
        time_t connection_time;

        // auth stuff
        time_t auth_expiration;
        char username[PLAYER_NAME_MAX_LENGTH+1];
        UserID user_id;
        
        int auth_attempts;

        int clock_time_tick;

    void was_authorized(UserID user_id, time_t expiration_time, const char* username);

    void player_data_was_loaded();
    void container_was_loaded(int container_id);
        
    bool failed_to_authorize();
    bool authorization_expired();
    void failed_authorization_attempt();
    
    void broadcast_disconnect();

    void init(int client_id);
    void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
