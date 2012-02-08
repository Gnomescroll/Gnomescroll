#include "net_CtoS.hpp"

#ifdef DC_SERVER

int _set_broadcast(int x, int y, int z, int value) {
    block_StoC msg;
    int i,j=0;
    i = _get(x,y,z);
    if (i != value) {
        j = _set(x,y,z, value);
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.val = value;
        msg.broadcast();
    }
    return j;
}

void _block_broadcast(int x, int y, int z, int value) {
    block_StoC msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.val = value;
    msg.broadcast();
}

void send_map_metadata(int client_id)
{
    map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}

void send_map_metadata()
{
    map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.broadcast();
}

void set_map_size(int x, int y, int z)
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
    send_map_metadata();
}


    inline void block_CtoS::handle() 
    {
        _set_broadcast(x,y,z, val);
    }
    inline void block_StoC::handle() {}
    inline void map_metadata_StoC::handle() {}

#endif

