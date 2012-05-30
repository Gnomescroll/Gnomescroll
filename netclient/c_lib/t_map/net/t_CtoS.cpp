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
    broadcast_set_block(x,y,z, val);
}
#endif

}   // t_map
