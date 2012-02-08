#include "net_StoC.hpp"

#ifdef DC_CLIENT
#include <c_lib/animations/animations.hpp>
#include <c_lib/common/random.h>
inline void block_StoC::handle() {
    if (val == 0) 
    {
        int cube_id = _get(x,y,z);
        Animations::block_crumble((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, randrange(10,30), cube_id);
    }
    _set(x,y,z,val);
}

inline void map_metadata_StoC::handle() {
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
}

inline void block_CtoS::handle(){}
#endif