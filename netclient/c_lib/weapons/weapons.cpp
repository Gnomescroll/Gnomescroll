#include "weapons.hpp"

#include <c_lib/weapons/packets.hpp>

namespace Weapons {

void Weapon::restore_ammo()
{
    #ifdef DC_SERVER
    if (this->ammo != max_ammo) {
        Agent_state* a = ServerState::agent_list.get(owner);
        if (a!=NULL) {
            WeaponAmmo_StoC ammo_msg;
            ammo_msg.type = type;
            ammo_msg.ammo = max_ammo;
            ammo_msg.sendToClient(a->client_id);
        }
    }
    #endif

    this->ammo = max_ammo;
}

bool HitscanLaser::fire() {
    if (clip == 0) return false;
    #ifdef DC_SERVER
    clip--;
    Agent_state* a = ServerState::agent_list.get(owner);
    if (a!=NULL) {
        WeaponClip_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.clip = clip;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

bool BlockApplier::fire() {
    if (ammo == 0) return false;
    #ifdef DC_SERVER
    ammo--;
    Agent_state* a = ServerState::agent_list.get(owner);
    if (a!=NULL) {
        WeaponAmmo_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.ammo = ammo;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

bool BlockPick::fire() {
    return true;
}

bool GrenadeThrower::fire() {
    if (ammo == 0) return false;
    #ifdef DC_SERVER
    ammo--;
    Agent_state* a = ServerState::agent_list.get(owner);
    if (a!=NULL) {
        WeaponAmmo_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.ammo = ammo;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

void HitscanLaser::reload() {
    #ifdef DC_SERVER
    int clip_used = clip_size - clip;
    // reload amt is lesser of the two: filling the clip or remaining ammo
    int amt = (ammo < clip_used) ? ammo : clip_used;
    if (amt == 0) return;
    ammo -= amt;
    clip += amt;
    Agent_state* a = ServerState::agent_list.get(owner);
    if (a==NULL) return;
    WeaponClip_StoC clip_msg;
    clip_msg.type = type;
    clip_msg.clip = clip;
    clip_msg.sendToClient(a->client_id);

    WeaponAmmo_StoC ammo_msg;
    ammo_msg.type = type;
    ammo_msg.ammo = ammo;
    ammo_msg.sendToClient(a->client_id);
    #endif
}

void HitscanLaser::restore_ammo()
{

    #ifdef DC_SERVER
    Agent_state* a = ServerState::agent_list.get(owner);
    if (a!=NULL) {
        if (this->clip != clip_size) {
            WeaponClip_StoC clip_msg;
            clip_msg.type = type;
            clip_msg.clip = clip_size;
            clip_msg.sendToClient(a->client_id);
        }
        if (this->ammo != max_ammo) {
            WeaponAmmo_StoC ammo_msg;
            ammo_msg.type = type;
            ammo_msg.ammo = max_ammo;
            ammo_msg.sendToClient(a->client_id);
        }
    }
    #endif

    this->ammo = max_ammo;
    this->clip = clip_size;
}



}
