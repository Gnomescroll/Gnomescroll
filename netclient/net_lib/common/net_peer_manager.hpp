#pragma once

class NetPeerManager
{
    public:
        int client_id;
        bool inited;
        bool loaded;

        void init(int client_id);
        void ready();
        void teardown();

        NetPeerManager();
};
