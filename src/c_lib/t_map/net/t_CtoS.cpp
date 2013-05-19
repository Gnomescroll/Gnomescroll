#include "t_CtoS.hpp"

#include <t_map/t_map.hpp>
#include <t_map/net/t_StoC.hpp>

namespace t_map
{

#if DC_CLIENT
inline void request_block_damage_CtoS::handle() {}
#endif

#if DC_SERVER

inline void request_block_damage_CtoS::handle()
{
    block_damage_StoC msg;
    int dmg = t_map::get_block_damage(this->position);
    if (dmg <= 0) return;
    msg.request_id = this->request_id;
    msg.dmg = dmg;
    msg.sendToClient(client_id);
}
#endif

}   // t_map
