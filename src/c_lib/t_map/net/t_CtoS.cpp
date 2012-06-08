#include "t_CtoS.hpp"

#include <t_map/t_map.hpp>

namespace t_map
{

#if DC_CLIENT
inline void block_CtoS::handle() {}
#endif

#if DC_SERVER
inline void block_CtoS::handle() 
{
    GS_ASSERT(x >= 0 && x < map_dim.x && y >= 0 && y < map_dim.y);
    if (x < 0 || x >= map_dim.x || y < 0 || y >= map_dim.y || z < 0 || z >= map_dim.z) return;

    broadcast_set_block(x,y,z, val);
}
#endif

}   // t_map
