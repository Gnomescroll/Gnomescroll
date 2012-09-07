#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

class NetPeerManager
{
    public:
        int client_id;
        bool inited;
        bool loaded;
        bool waiting_for_auth;
        
        time_t connection_time;

        // auth stuff
        time_t auth_expiration;
        char* username;
        int user_id;

        void authorized(int user_id, time_t expiration_time, const char* username);

        void init(int client_id);
        void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
