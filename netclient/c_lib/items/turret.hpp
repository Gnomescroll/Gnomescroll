#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>
#include <c_lib/components/components.hpp>

//forward decl
#if DC_SERVER
namespace ServerState
{
    extern OwnedList* owned_list;
}
#endif
#if DC_CLIENT
namespace ClientState
{
    extern OwnedList* owned_list;
}
#endif

// forward decl
class object_shot_object_StoC;
class object_shot_terrain_StoC;
class object_shot_nothing_StoC;

void turret_shot_object(object_shot_object_StoC* msg);
void turret_shot_terrain(object_shot_terrain_StoC* msg);
void turret_shot_nothing(object_shot_nothing_StoC* msg);

extern VoxDat turret_vox_dat;

/* TargetAcquistion component */
// TODO: move to c_lib/components/
class TargetAcquisitionComponent
{
    public:
        float target_acquisition_probability;
        Hitscan::AttackerProperties attacker_properties;
        
        void acquire_target(
            int id, Object_types type, int team, float camera_z, Vec3 position,
            float accuracy_bias, float sight_range,
            bool attack_enemies, bool attack_random
        );
    TargetAcquisitionComponent(){}
};

typedef ObjectInterface
< OwnedTeamHealthPositionVoxelState, object_create_owner_team_StoC, object_state_StoC >
TurretInterface;

class Turret: public TargetAcquisitionComponent, public VoxelComponent, public TurretInterface
{
    public:
        explicit Turret(int id)
        : TargetAcquisitionComponent(), TurretInterface()
        {
            this->_state.id = id;
            this->_state.type = OBJ_TYPE_TURRET;

            this->_state.broadcast_state_change = true;
            this->_state.blow_up_on_death = true;
            this->_state.explosion_radius = TURRET_EXPLOSION_RADIUS;
            this->_state.explosion_damage = TURRET_EXPLOSION_DAMAGE;
            this->_state.cost = COST_TURRET;
            this->_state.reward = COST_TURRET;
            this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
            
            this->_state.suicidal = false;

            this->_state.fire_rate_limit = TURRET_FIRE_LIMIT;
            this->_state.sight_range = TURRET_SIGHT_RANGE;
            this->_state.accuracy_bias = TURRET_LASER_BIAS;
            this->_state.attack_enemies = true;
            this->_state.attack_random = true;

            this->attacker_properties.agent_protection_duration = AGENT_TURRET_PROTECTION_DURATION;
            this->attacker_properties.agent_damage = TURRET_AGENT_DAMAGE;
            this->attacker_properties.block_damage = TURRET_BLOCK_DAMAGE;
            this->attacker_properties.voxel_damage_radius = TURRET_LASER_VOXEL_DAMAGE_RADIUS;
            this->attacker_properties.terrain_modification_action = t_map::TMA_LASER;
            this->target_acquisition_probability = TURRET_TARGET_LOCK_CHANCE;

            this->owned_properties.obj = this;
            STATE::owned_list->register_object(&this->owned_properties);

            this->team_properties.obj = this;
            //STATE::team_list->register_object(&this->team_properties);

            this->health_properties.health = TURRET_HEALTH;

            this->voxel_properties.frozen_vox = true;
            this->voxel_properties.init_hitscan = true;
            this->voxel_properties.init_draw = true;
            this->voxel_properties.vox_dat = &turret_vox_dat;

            this->position_properties.camera_height = TURRET_CAMERA_HEIGHT;
            this->position_properties.height = TURRET_HEIGHT;
        }

        ~Turret()
        {
            STATE::owned_list->unregister_object(&this->owned_properties);
        }

    /* Interface */

    Turret* get_derived() { return this; }

    void tick()
    {   // make each a template function
        tickStayOnGround(this->state(), this);
        tickTargetAcquisition(this->state(), this, this->camera_z());
    }

    void update()
    {
        updateFrozenVox(
            this->voxel_properties.vox, this->get_position(),
            this->position_properties.theta, this->position_properties.phi,
            this->position_properties.changed()
        );
    }

    void draw() {}

    void born()
    {
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornTeamVox(this->voxel_properties.vox_dat, state->id, state->type, this->team_properties.team);
        bornSetVox(this->voxel_properties.vox, this->voxel_properties.init_hitscan, this->voxel_properties.init_draw);
        bornUpdateFrozenVox(this->voxel_properties.vox, this->get_position(), this->position_properties.theta, this->position_properties.phi);
        bornCreateMessage(this);
    }

    void die()
    {
        ObjectState* state = this->state();
        dieExplode(state, this);
        dieBroadcast(state, this);
        dieRevokeOwner(state, this);
        if (this->voxel_properties.vox != NULL)
            dieTeamItemAnimation(this->voxel_properties.vox->get_part(0)->get_center(), this->team_properties.team);
    }

};
