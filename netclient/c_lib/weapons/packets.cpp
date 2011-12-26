#include "packets.hpp"

#include <c_lib/weapons/weapons.hpp>

namespace Weapons {

#ifdef DC_CLIENT
inline void WeaponAmmo_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(ClientState::playerAgent_state.agent_id);
    if (a==NULL) return;
    a->weapons.set_ammo(type, ammo);
}

inline void WeaponClip_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(ClientState::playerAgent_state.agent_id);
    if (a==NULL) return;
    a->weapons.set_clip(type, clip);
}

#endif

#ifdef DC_SERVER
inline void WeaponAmmo_StoC::handle() {}
inline void WeaponClip_StoC::handle() {}
#endif

}
