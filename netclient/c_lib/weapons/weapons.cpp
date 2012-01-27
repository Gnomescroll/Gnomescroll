#include "weapons.hpp"

#include <c_lib/weapons/packets.hpp>

namespace Weapons {

static const char hud_undefined_string[] = "--";
static const char hud_display_format_string[] = "%s/%s::%s/%s";


Weapon::Weapon(weapon_type type) :
max_ammo(0), speed(1),
id(0),
owner(0),
type(type),
ammo(0),
scope(false)
{}


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

/* Hitscan laser */

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

char* HitscanLaser::hud_display() {
    char clip_str[4+1];
    sprintf(clip_str, "%d", clip);
    char clip_size_str[4+1];
    sprintf(clip_size_str, "%d", clip_size);
    char ammo_str[4+1];
    sprintf(ammo_str, "%d", ammo);
    char max_ammo_str[4+1];
    sprintf(max_ammo_str, "%d", max_ammo);
    
    sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

HitscanLaser::HitscanLaser()
:
Weapon(TYPE_hitscan_laser),
clip_size(100),
clip(100)
{
    this-> max_ammo = 100;
    this->ammo = 100;
    this->scope = true;
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

/* Block applier */

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

char* BlockApplier::hud_display() {
    const char* clip_str = hud_undefined_string;
    const char* clip_size_str = hud_undefined_string;
    char ammo_str[4+1];
    sprintf(ammo_str, "%d", ammo);
    char max_ammo_str[4+1];
    sprintf(max_ammo_str, "%d", max_ammo);
    
    sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}


BlockApplier::BlockApplier()
:
Weapon(TYPE_block_applier),
block(2)
{
    this->max_ammo = 9999;
    this->ammo = 9999;
}

/* Pick */

bool BlockPick::fire() {
    return true;
}

char* BlockPick::hud_display() {
    const char* clip_str = hud_undefined_string;
    const char* clip_size_str = hud_undefined_string;
    const char* ammo_str = hud_undefined_string;
    const char* max_ammo_str = hud_undefined_string;
    sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

BlockPick::BlockPick() :
Weapon(TYPE_block_pick)
{}


/* Grenade thrower */

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

char* GrenadeThrower::hud_display() {
    const char* clip_str = hud_undefined_string;
    const char* clip_size_str = hud_undefined_string;
    char ammo_str[4+1];
    sprintf(ammo_str, "%d", ammo);
    char max_ammo_str[4+1];
    sprintf(max_ammo_str, "%d", max_ammo);
    
    sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

GrenadeThrower::GrenadeThrower()
:
Weapon(TYPE_grenade_thrower)
{
    this->max_ammo = 9999;
    this->ammo = 9999;
}

}
