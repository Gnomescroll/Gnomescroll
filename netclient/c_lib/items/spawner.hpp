#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

extern VoxDat spawner_vox_dat;

/* Spawner Component */

class SpawnerComponent
{
    public:
    void get_spawn_point(ObjectState* state, int spawned_object_height, int* spawn_pt);
    SpawnerComponent(){}

};

class Spawner; // forward decl

typedef TickStayOnGround < NoTick(Spawner), Spawner>
    SpawnerTick;

typedef UpdateFrozenVox < NoUpdate(Spawner), Spawner>
    SpawnerUpdate;

typedef BornTeamVox < BornSetVox < BornUpdateFrozenVox < BornCreateMessage < NoBorn(Spawner), Spawner>,Spawner>,Spawner>,Spawner>
    SpawnerBorn;

typedef DieBroadcast < DieRevokeOwner < DieTeamItemAnimation < NoDie(Spawner), Spawner>,Spawner>,Spawner>
    SpawnerDie;

typedef ObjectPolicy
< Spawner, SpawnerTick, NoDraw(Spawner), SpawnerUpdate, SpawnerBorn, SpawnerDie,
    object_create_owner_team_index_StoC, object_state_StoC >
SpawnerObjectParent;

class Spawner: public SpawnerObjectParent, public SpawnerComponent
{
    public:
        Spawner(int id, float x, float y, float z, float mx, float my, float mz)
        : SpawnerObjectParent(this), SpawnerComponent()
        {
            this->_state.id = id;
            this->_state.position = vec3_init(x,y,z);
            this->_state.height = SPAWNER_HEALTH;
            this->_state.broadcast_state_change = true;
            this->_state.cost = COST_SPAWNER;
            this->_state.reward = COST_SPAWNER;
            this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
            this->_state.frozen_vox = true;
            this->_state.vox_dat = &spawner_vox_dat;
            this->_state.type = OBJ_TYPE_SPAWNER;
            this->_state.health = SPAWNER_HEALTH;
            this->_state.spawn_radius = SPAWNER_SPAWN_RADIUS;
        }
};
