#include "net_CtoS.hpp"

#ifdef DC_SERVER

    inline void block_CtoS::handle() 
    {
        _set_broadcast(x,y,z, val);
    }
    inline void block_StoC::handle() {}
    inline void map_metadata_StoC::handle() {}

#endif

