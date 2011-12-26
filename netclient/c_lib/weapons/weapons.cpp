#include "weapons.hpp"

#include <c_lib/weapons/packets.hpp>

namespace Weapons {

bool HitscanLaser::fire() {
    if (clip == 0) return false;
    clip--;
    return true;
}

bool BlockApplier::fire() {
    if (ammo == 0) return false;
    ammo--;
    return true;
}

bool BlockPick::fire() {
    return true;
}

bool GrenadeThrower::fire() {
    if (ammo == 0) return false;
    ammo--;
    return true;
}


void HitscanLaser::reload() {
    printf("clip_size=%d, clip=%d\n", clip_size, clip);
    printf("max_ammo=%d, ammo=%d\n", max_ammo, ammo);
    int clip_remaining = clip_size - clip;
    // reload amt is lesser of the two: filling the clip or remaining ammo
    int amt = (ammo < clip_remaining) ? ammo : clip_remaining;
    ammo -= amt;
    clip += amt;
    printf("reloaded, amt=%d\n", amt);
    #ifdef DC_SERVER
    if (amt != 0) {
        Agent_state* a = ServerState::agent_list.get(owner);
        static WeaponClip_StoC clip_msg;
        clip_msg.type = type;
        clip_msg.clip = clip;
        clip_msg.sendToClient(a->client_id);

        static WeaponAmmo_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.ammo = ammo;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
}

}
