#pragma once

class NetPeerManager
{
    public:
        int client_id;
        bool inited;
        bool loaded;

        void ready();
        void init(int client_id);
        void teardown();

        NetPeerManager();
};
