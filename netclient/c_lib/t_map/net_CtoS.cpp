#include "net_CtoS.hpp"

#include "net_StoC.hpp"

#ifdef DC_SERVER

void _set_broadcast(int x, int y, int z, int value) {
    class block_StoC msg;
    int i = _get(x,y,z);
    if (i != value) 
    {
        _set(x,y,z, value);
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.val = value;
        msg.broadcast();
    }
}

void _block_broadcast(int x, int y, int z, int value) {
    class block_StoC msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.val = value;
    msg.broadcast();
}

void send_map_metadata(int client_id)
{
    class map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}

void send_map_metadata()
{
    class map_metadata_StoC msg;
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

