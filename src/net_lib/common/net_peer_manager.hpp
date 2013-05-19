#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <agent/constants.hpp>
#include <item/common/constants.hpp>

#if GS_SERIALIZER
# include <serializer/players.hpp>
#endif


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

    bool failed_to_authorize();
    bool authorization_expired(time_t now);
    void failed_authorization_attempt();

    void deserializer_failed();

    #if GS_SERIALIZER
    void was_deserialized(class serializer::ParsedPlayerData* data);
    #else
    void was_deserialized();
    #endif

    void broadcast_disconnect();

    void init(ClientID client_id);
    void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
