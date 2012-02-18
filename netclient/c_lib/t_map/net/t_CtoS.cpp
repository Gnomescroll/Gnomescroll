#include "t_CtoS.hpp"

#include "t_StoC.hpp"

#include "../t_map.hpp"

using namespace t_map;

#ifdef DC_SERVER

void block_CtoS::handle() 
{
    _set_broadcast(x,y,z, val);
}

#endif

#ifdef DC_CLIENT

void block_CtoS::handle() {}

#endif