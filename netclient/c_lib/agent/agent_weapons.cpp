#include "agent_weapons.hpp"

static const int N_AGENT_WEAPONS = 6;

Agent_weapons::Agent_weapons(Agent_state* a)
: a(a), active(0)
{
    laser.owner = a->id;
    pick.owner = a->id;
    blocks.owner = a->id;
    grenades.owner = a->id;
    spawner.owner = a->id;
    turret.owner = a->id;
}

void Agent_weapons::set_ammo(int type, int ammo) {

    int old;

    switch (type) {
        case Weapons::TYPE_block_applier:
            old = blocks.ammo;
            blocks.ammo = ammo;
            break;
        //case Weapons::TYPE_block_pick:
            //break;
        case Weapons::TYPE_grenade_thrower:
            old = grenades.ammo;
            grenades.ammo = ammo;
            break;
        case Weapons::TYPE_hitscan_laser:
            old = laser.ammo;
            laser.ammo = ammo;
            break;
        default:
            printf("Agent_weapons::set_ammo unknown or invalid weapon type %d\n", type);
            return;
    }

    if (ammo > old)
    {
        #ifdef DC_CLIENT
        if (a->is_you())
            Sound::restore_ammo();
        else
            Sound::restore_ammo(
                a->s.x,
                a->s.y,
                a->s.z,
                0,0,0
            );
        #endif
    }

}

void Agent_weapons::set_clip(int type, int clip) {

    switch (type) {
        //case Weapons::TYPE_block_applier:
            //break;
        //case Weapons::TYPE_block_pick:
            //break;
        //case Weapons::TYPE_grenade_thrower:
            //break;
        case Weapons::TYPE_hitscan_laser:
            laser.clip = clip;
            break;
        default:
            printf("Agent_weapons::set_clip unknown or invalid weapon type %d\n", type);
            return;
    }

}

void Agent_weapons::set_active(int slot) {
    if (slot < 0 || slot >= N_AGENT_WEAPONS) return;
    active = slot;

    #ifdef DC_CLIENT
    if (a->id == ClientState::playerAgent_state.agent_id) {
        weapon_change_message();
    }
    #endif

    #ifdef DC_SERVER
    weapon_change_message();
    #endif
}

void Agent_weapons::switch_up() {
    active = (active+1)%N_AGENT_WEAPONS;
    weapon_change_message();
}

void Agent_weapons::switch_down() {
    active = (active-1)%N_AGENT_WEAPONS;
    if (active < 0)
        active += N_AGENT_WEAPONS;
    weapon_change_message();
}

void Agent_weapons::weapon_change_message() {
    #ifdef DC_CLIENT
    AgentActiveWeapon_CtoS msg;
    msg.slot = active;
    msg.send();
    #endif
    #ifdef DC_SERVER
    AgentActiveWeapon_StoC msg;
    msg.id = this->a->id;
    msg.slot = active;
    msg.broadcast();
    #endif
}

typedef enum {
     LASER_SLOT,
     PICK_SLOT,
     BLOCKS_SLOT,
     GRENADES_SLOT,
     SPAWNER_SLOT,
     TURRET_SLOT
} AgentWeaponSlots;

bool Agent_weapons::is_active(int type) {
    bool res = false;
    switch (active) {
        case LASER_SLOT:
            res = (laser.type == type);
            break;
        case PICK_SLOT:
            res = (pick.type == type);
            break;
        case BLOCKS_SLOT:
            res = (blocks.type == type);
            break;
        case GRENADES_SLOT:
            res = (grenades.type == type);
            break;
        case SPAWNER_SLOT:
            res = (spawner.type == type);
            break;
        case TURRET_SLOT:
            res = (turret.type == type);
            break;
        default:
            printf("Agent_weapons::is_active critical error. active weapon %d out of range\n", active);
            break;
    }
    return res;
}

//void Agent_weapons::adjust_active() {
    //static const int n = 4; // # weapons

    //active = active % n;
    //if (active < 0) active += n-1;
//}

void Agent_weapons::reload() {
    switch (active) {
        //case Weapons::TYPE_block_applier:
            //blocks.ammo = ammo;
            //break;
        //case Weapons::TYPE_block_pick:
            //break;
        //case Weapons::TYPE_grenade_thrower:
            //grenades.ammo = ammo;
            //break;
        case Weapons::TYPE_hitscan_laser:
            laser.reload();
            break;
        default:
            //printf("Agent_weapons::reload unknown or invalid weapon type %d\n", type);
            return;
    }
}

void Agent_weapons::reload(int type) {
    switch (type) {
        //case Weapons::TYPE_block_applier:
            //blocks.ammo = ammo;
            //break;
        //case Weapons::TYPE_block_pick:
            //break;
        //case Weapons::TYPE_grenade_thrower:
            //grenades.ammo = ammo;
            //break;
        case Weapons::TYPE_hitscan_laser:
            laser.reload();
            break;
        default:
            return;
    }
}

char* Agent_weapons::hud_display() {
    char* str = NULL;
    int type = active_type();
    switch (type) {
        case Weapons::TYPE_block_applier:
            str = blocks.hud_display();
            break;
        case Weapons::TYPE_block_pick:
            str = pick.hud_display();
            break;
        case Weapons::TYPE_grenade_thrower:
            str = grenades.hud_display();
            break;
        case Weapons::TYPE_hitscan_laser:
            str = laser.hud_display();
            break;
        case Weapons::TYPE_spawner_placer:
            str = spawner.hud_display();
            break;
        case Weapons::TYPE_turret_placer:
            str = turret.hud_display();
            break;
        default:
            printf("Agent_weapons::hud_display unknown or invalid weapon type %d\n", type);
            str = (char*)"";
            break;
    }
    return str;
}

int Agent_weapons::active_type() {
    int t = -1;
    switch (active) {
        case BLOCKS_SLOT:
            t = blocks.type;
            break;
        case PICK_SLOT:
            t = pick.type;
            break;
        case GRENADES_SLOT:
            t = grenades.type;
            break;
        case LASER_SLOT:
            t = laser.type;
            break;
        case SPAWNER_SLOT:
            t = spawner.type;
            break;
        case TURRET_SLOT:
            t = turret.type;
            break;
        default:
            printf("Agent_weapons::active_type unknown or invalid weapon slot %d\n", active);
            break;
    }
    return t;
}

void Agent_weapons::set_active_block(int block) {
    blocks.block = block;
}

bool Agent_weapons::fire() {
    bool fired = false;
    int type = active_type();
    switch (type) {
        case Weapons::TYPE_block_applier:
            fired = blocks.fire();
            break;
        case Weapons::TYPE_block_pick:
            fired = pick.fire();
            break;
        case Weapons::TYPE_grenade_thrower:
            fired = grenades.fire();
            break;
        case Weapons::TYPE_hitscan_laser:
            fired = laser.fire();
            break;
        case Weapons::TYPE_spawner_placer:
            fired = spawner.fire();
            break;
        case Weapons::TYPE_turret_placer:
            fired = turret.fire();
            break;
        default:
            printf("Agent_weapons::fire -- unknown or invalid weapon type %d\n", type);
            break;
    }
    return fired;
}

bool Agent_weapons::fire(int type) {
    bool fired = false;
    switch (type) {
        case Weapons::TYPE_block_applier:
            fired = blocks.fire();
            break;
        case Weapons::TYPE_block_pick:
            fired = pick.fire();
            break;
        case Weapons::TYPE_grenade_thrower:
            fired = grenades.fire();
            break;
        case Weapons::TYPE_hitscan_laser:
            fired = laser.fire();
            break;
        case Weapons::TYPE_spawner_placer:
            fired = spawner.fire();
            break;
        case Weapons::TYPE_turret_placer:
            fired = turret.fire();
            break;
        default:
            printf("Agent_weapons::fire -- unknown or invalid weapon type %d\n", type);
            break;
    }
    return fired;
}

bool Agent_weapons::can_zoom() {
    bool zoom = false;
    switch (active) {
        case BLOCKS_SLOT:
            zoom = blocks.scope;
            break;
        case PICK_SLOT:
            zoom = pick.scope;
            break;
        case GRENADES_SLOT:
            zoom = grenades.scope;
            break;
        case LASER_SLOT:
            zoom = laser.scope;
            break;
        case SPAWNER_SLOT:
            zoom = spawner.scope;
            break;
        case TURRET_SLOT:
            zoom = turret.scope;
            break;
        default:
            printf("Agent_weapons::can_zoom unknown or invalid weapon slot %d\n", active);
            break;
    }
    return zoom;
}

void Agent_weapons::restore_ammo()
{
    this->blocks.restore_ammo();
    this->laser.restore_ammo();
    this->grenades.restore_ammo();
    //this->pick.restore_ammo();
}
