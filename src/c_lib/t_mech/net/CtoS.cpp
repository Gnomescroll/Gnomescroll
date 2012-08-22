#include "CtoS.hpp"

#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>

namespace t_mech
{
/*
#if DC_CLIENT
inline void request_block_damage_CtoS::handle() {}
#endif

#if DC_SERVER

inline void request_block_damage_CtoS::handle()
{
    block_damage_StoC msg;
    int dmg = t_map::get_block_damage(x,y,z);
    if (dmg <= 0) return;
    msg.request_id = this->request_id;
    msg.dmg = dmg;
    msg.sendToClient(client_id);
}
#endif
*/

}   // t_mech
