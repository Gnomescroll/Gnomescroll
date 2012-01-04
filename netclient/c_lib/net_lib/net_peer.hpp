#pragma once


/*
NetPeer
*/

class NetPeer
{
    private:
    public:
    int client_id;
    int connected;

    //remote server
    //intSocket* socket;

    uint32_t ip;
    uint16_t port;
    ENetPeer * peer;

    int pending_bytes;

    int bn; //buffer offset
    char* buffer; //4096 elements

    void flush_to_net();
    void receive_packets();
    int check_for_error();

    NetPeer() 
    {
        buffer = new char[4096]; //put into pool, so it goes into L2 cache
        client_id = -1;
        bn = 6;
        pending_bytes = 0;
    }

    ~NetPeer()
    {
        delete buffer;
    }
    //create netpeer from listener
};

/*
struct Socket* create_socket(uint16_t port);

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port);
class NetPeer* create_net_peer_from_address(struct sockaddr_in address);
*/