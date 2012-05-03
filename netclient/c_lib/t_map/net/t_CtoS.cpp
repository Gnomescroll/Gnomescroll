#include "t_CtoS.hpp"

#include "t_StoC.hpp"

#include "../t_map.hpp"

namespace t_map
{

#if DC_SERVER

void block_CtoS::handle() 
{
    _set_broadcast(x,y,z, val);
}

#endif

#if DC_CLIENT

void block_CtoS::handle() {}

#endif

}   // t_map
