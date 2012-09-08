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
        bool authorized;
        
        time_t connection_time;

        // auth stuff
        time_t auth_expiration;
        char* username;
        int user_id;

    void was_authorized(int user_id, time_t expiration_time, const char* username);

    bool failed_to_authorize();
    bool authorization_expired();

    void init(int client_id);
    void teardown();

    ~NetPeerManager();
    NetPeerManager();
};
