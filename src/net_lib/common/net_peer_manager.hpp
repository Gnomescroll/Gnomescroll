#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <agent/constants.hpp>
#include <item/common/constants.hpp>

class NetPeerManager
{
    public:
        ClientID client_id;
        AgentID agent_id;
        bool inited;
        bool loaded;
        bool waiting_for_auth;
        bool authorized;
        bool deserialized;
        
        time_t connection_time;

        // auth stuff
        time_t auth_expiration;
        char username[PLAYER_NAME_MAX_LENGTH+1];
        UserID user_id;
        
        int auth_attempts;

        int clock_time_tick;

    void was_authorized(UserID user_id, time_t expiration_time, const char* username);

    void was_deserialized();    // this will be called internally once everything expected has been loaded
        
    bool failed_to_authorize();
    bool authorization_expired();
    void failed_authorization_attempt();
    
    void broadcast_disconnect();

    void init(ClientID client_id);
    void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
