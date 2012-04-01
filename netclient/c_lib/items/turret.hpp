#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

// forward decl
class object_shot_object_StoC;
class object_shot_terrain_StoC;
class object_shot_nothing_StoC;

void turret_shot_object(object_shot_object_StoC* msg);
void turret_shot_terrain(object_shot_terrain_StoC* msg);
void turret_shot_nothing(object_shot_nothing_StoC* msg);

extern VoxDat turret_vox_dat;

/* TargetAcquistion component */

class TargetAcquisitionComponent
{
    public:
    void acquire_target(ObjectState* state);
    TargetAcquisitionComponent(){}
};

class Turret; // forward decl

typedef TickStayOnGround < TickTargetAcquisition < NoTick(Turret), Turret>,Turret>
    TurretTick;

typedef UpdateFrozenVox < NoUpdate(Turret), Turret> TurretUpdate;

typedef BornTeamVox < BornSetVox < BornUpdateFrozenVox < BornCreateMessage < NoBorn(Turret), Turret>,Turret>,Turret>,Turret>
    TurretBorn;

typedef DieExplode < DieBroadcast < DieRevokeOwner < DieTeamItemAnimation < NoDie(Turret), Turret>,Turret>,Turret>,Turret> TurretDie;

typedef ObjectPolicy
< Turret, TurretTick, NoDraw(Turret), TurretUpdate, TurretBorn, TurretDie,
    object_create_owner_team_StoC, object_state_StoC >
TurretObjectParent;

class Turret: public TurretObjectParent, public TargetAcquisitionComponent
{
    public:
        Turret(int id)
        : TurretObjectParent(this), TargetAcquisitionComponent()
        {
            this->_state.id = id;
            this->_state.height = TURRET_HEIGHT;
            this->_state.camera_height = TURRET_CAMERA_HEIGHT;
            this->_state.broadcast_state_change = true;
            this->_state.blow_up_on_death = true;
            this->_state.explosion_radius = TURRET_EXPLOSION_RADIUS;
            this->_state.explosion_damage = TURRET_EXPLOSION_DAMAGE;
            this->_state.cost = COST_TURRET;
            this->_state.reward = COST_TURRET;
            this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
            this->_state.fire_rate_limit = TURRET_FIRE_LIMIT;
            this->_state.sight_range = TURRET_SIGHT_RANGE;
            this->_state.accuracy_bias = TURRET_LASER_BIAS;
            this->_state.target_acquisition_probability = TURRET_TARGET_LOCK_CHANCE;
            this->_state.attack_enemies = true;
            this->_state.attack_random = true;
            this->_state.suicidal = false;
            this->_state.frozen_vox = true;
            this->_state.vox_dat = &turret_vox_dat;
            this->_state.type = OBJ_TYPE_TURRET;
            this->_state.health = TURRET_HEALTH;
            this->_state.attacker_properties.agent_protection_duration = AGENT_TURRET_PROTECTION_DURATION;
            this->_state.attacker_properties.agent_damage = TURRET_AGENT_DAMAGE;
            this->_state.attacker_properties.block_damage = TURRET_BLOCK_DAMAGE;
            this->_state.attacker_properties.voxel_damage_radius = TURRET_LASER_VOXEL_DAMAGE_RADIUS;
            this->_state.attacker_properties.terrain_modification_action = t_map::TMA_LASER;
        }
};
