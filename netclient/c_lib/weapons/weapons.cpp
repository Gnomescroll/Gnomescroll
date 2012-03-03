#include "weapons.hpp"

#include <c_lib/weapons/packets.hpp>

namespace Weapons {

static const char hud_undefined_string[] = "--";
static const char hud_display_format_string[] = "%s/%s::%s/%s";
static const char out_of_ammo_string[] = "OUT OF AMMO";
static const char reload_string[] = "RELOAD";

Weapon::Weapon(weapon_type type)
:
clip_str(NULL),
clip_size_str(NULL),
ammo_str(NULL),
max_ammo_str(NULL),
max_ammo(0),
speed(1),
id(0),
owner(0),
type(type),
ammo(0),
scope(false)
{
    this->hud_string = (char*)calloc(WEAPON_HUD_STRING_MAX+1, sizeof(char));
}

Weapon::~Weapon()
{
    free(this->hud_string);
}

void Weapon::restore_ammo()
{
    #ifdef DC_SERVER
    if (this->ammo != max_ammo) {
        Agent_state* a = ServerState::agent_list->get(owner);
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
    Agent_state* a = ServerState::agent_list->get(owner);
    if (a!=NULL) {
        WeaponClip_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.clip = clip;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

char* HitscanLaser::hud_display()
{
    if (clip <= 0)
    {
        if (ammo <= 0)
            return const_cast<char*>(out_of_ammo_string);
        return const_cast<char*>(reload_string);
    }

    if (clip > 9999) clip = 9999;
    if (clip < -999) clip = -999;
    //sprintf(clip_str, "%d", clip);
    
    //if (clip_size > 9999) clip_size = 9999;
    //if (clip_size < -999) clip_size = -999;
    //sprintf(clip_size_str, "%d", clip_size);

    if (ammo > 9999) ammo = 9999;
    if (ammo < -999) ammo = -999;
    //sprintf(ammo_str, "%d", ammo);

    //if (max_ammo > 9999) max_ammo = 9999;
    //if (max_ammo < -999) max_ammo = -999;
    //sprintf(max_ammo_str, "%d", max_ammo);

    sprintf(hud_string, "%d/%d charges", clip, ammo);
    //sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

HitscanLaser::HitscanLaser()
:
Weapon(TYPE_hitscan_laser),
clip_size(100),
clip(100)
{
    this->max_ammo = 100;
    this->ammo = 100;
    this->scope = true;
    this->clip_str = (char*)calloc(4+1, sizeof(char));
    this->clip_size_str = (char*)calloc(4+1, sizeof(char));
    this->ammo_str = (char*)calloc(4+1, sizeof(char));
    this->max_ammo_str = (char*)calloc(4+1, sizeof(char));
}

HitscanLaser::~HitscanLaser()
{
    free(this->clip_str);
    free(this->clip_size_str);
    free(this->ammo_str);
    free(this->max_ammo_str);
}

void HitscanLaser::reload() {
    #ifdef DC_SERVER
    int clip_used = clip_size - clip;
    // reload amt is lesser of the two: filling the clip or remaining ammo
    int amt = (ammo < clip_used) ? ammo : clip_used;
    if (amt == 0) return;
    ammo -= amt;
    clip += amt;
    Agent_state* a = ServerState::agent_list->get(owner);
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
    Agent_state* a = ServerState::agent_list->get(owner);
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
    Agent_state* a = ServerState::agent_list->get(owner);
    if (a!=NULL) {
        WeaponAmmo_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.ammo = ammo;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

char* BlockApplier::hud_display()
{
    if (ammo <= 0)
        return const_cast<char*>(out_of_ammo_string);

    if (ammo > 9999) ammo = 9999;
    if (ammo < -999) ammo = -999;
    //sprintf(ammo_str, "%d", ammo);

    //if (max_ammo > 9999) max_ammo = 9999;
    //if (max_ammo < -999) max_ammo = -999;
    //sprintf(max_ammo_str, "%d", max_ammo);

    sprintf(hud_string, "%d blocks", ammo);
    //sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

void BlockApplier::set_block(int block)
{
    this->block = block;
}

BlockApplier::BlockApplier()
:
Weapon(TYPE_block_applier),
block(2)
{
    #if PRODUCTION
    this->max_ammo = 200;
    this->ammo = 200;
    #else
    this->max_ammo = 9999;
    this->ammo = 9999;
    #endif

    this->clip_str = const_cast<char*>(hud_undefined_string);
    this->clip_size_str = const_cast<char*>(hud_undefined_string);
    this->ammo_str = (char*)calloc(4+1, sizeof(char));
    this->max_ammo_str = (char*)calloc(4+1, sizeof(char));
}

BlockApplier::~BlockApplier()
{
    free(this->ammo_str);
    free(this->max_ammo_str);
}

/* Pick */

bool BlockPick::fire() {
    return true;
}

char* BlockPick::hud_display()
{
    //sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

BlockPick::BlockPick() :
Weapon(TYPE_block_pick)
{
    sprintf(this->hud_string, (char*)"Disintegrator");
    this->clip_str = const_cast<char*>(hud_undefined_string);
    this->clip_size_str = const_cast<char*>(hud_undefined_string);
    this->ammo_str = const_cast<char*>(hud_undefined_string);
    this->max_ammo_str = const_cast<char*>(hud_undefined_string);
}

BlockPick::~BlockPick()
{}

/* Grenade thrower */

bool GrenadeThrower::fire() {
    if (ammo == 0) return false;
    #ifdef DC_SERVER
    ammo--;
    Agent_state* a = ServerState::agent_list->get(owner);
    if (a!=NULL) {
        WeaponAmmo_StoC ammo_msg;
        ammo_msg.type = type;
        ammo_msg.ammo = ammo;
        ammo_msg.sendToClient(a->client_id);
    }
    #endif
    return true;
}

char* GrenadeThrower::hud_display()
{
    if (ammo <= 0)
        return const_cast<char*>(out_of_ammo_string);

    if (ammo > 9999) ammo = 9999;
    if (ammo < -999) ammo = -999;
    //sprintf(ammo_str, "%d", ammo);

    //if (max_ammo > 9999) max_ammo = 9999;
    //if (max_ammo < -999) max_ammo = -999;
    //sprintf(max_ammo_str, "%d", max_ammo);

    sprintf(hud_string, "%d grenades", ammo);
    //sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

GrenadeThrower::GrenadeThrower()
:
Weapon(TYPE_grenade_thrower)
{
    #if PRODUCTION
    this->max_ammo = 100;
    this->ammo = 100;
    #else
    this->max_ammo = 9999;
    this->ammo = 9999;
    #endif
    this->clip_str = const_cast<char*>(hud_undefined_string);
    this->clip_size_str = const_cast<char*>(hud_undefined_string);
    this->ammo_str = (char*)calloc(4+1, sizeof(char));
    this->max_ammo_str = (char*)calloc(4+1, sizeof(char));
}

GrenadeThrower::~GrenadeThrower()
{
    free(this->ammo_str);
    free(this->max_ammo_str);
}

/* SpawnerPlacer (TEMPORARY) */

bool SpawnerPlacer::fire()
{
    return true;
}

char* SpawnerPlacer::hud_display()
{   // TODO: get # of spawners agent can afford, print that
    //sprintf(hud_string, hud_display_format_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
    return hud_string;
}

SpawnerPlacer::SpawnerPlacer()
:
Weapon(TYPE_spawner_placer)
{
    // doesnt use traditional ammo
    // set these values very high so that it shouldnt run out,
    // and any fire() tests will pass
    this->max_ammo = 9999;
    this->ammo = 9999;
    this->clip_str = const_cast<char*>(hud_undefined_string);
    this->clip_size_str = const_cast<char*>(hud_undefined_string);
    this->ammo_str = const_cast<char*>(hud_undefined_string);
    this->max_ammo_str = const_cast<char*>(hud_undefined_string);
    sprintf(this->hud_string, (char*)"Spawner synthesizer");
}

SpawnerPlacer::~SpawnerPlacer()
{}


}
