#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/entity/network/interfaces.hpp>

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

class Turret: public TargetAcquisitionComponent, public VoxelComponent, public ObjectStateLayer
{
    public:
        OwnedComponent owned;
        TeamComponent team;
        HealthComponent health;
        PositionChangedComponent spatial;

    explicit Turret(int id)
    : TargetAcquisitionComponent(), ObjectStateLayer(Objects::create_packet_owner_team, Objects::state_packet, &owned, &team, &health, &spatial)
    {
        this->_state.id = id;
        this->_state.type = OBJ_TYPE_TURRET;

        this->_state.blow_up_on_death = true;
        this->_state.explosion_radius = TURRET_EXPLOSION_RADIUS;
        this->_state.explosion_damage = TURRET_EXPLOSION_DAMAGE;
        this->_state.cost = COST_TURRET;
        this->_state.reward = COST_TURRET;
        this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
        
        this->_state.suicidal = false;

        this->_state.sight_range = TURRET_SIGHT_RANGE;
        this->_state.accuracy_bias = TURRET_LASER_BIAS;
        this->_state.attack_enemies = true;
        this->_state.attack_random = true;

        this->attacker_properties.id = id;
        this->attacker_properties.type = OBJ_TYPE_TURRET;
        this->attacker_properties.agent_protection_duration = AGENT_TURRET_PROTECTION_DURATION;
        this->attacker_properties.agent_damage = TURRET_AGENT_DAMAGE;
        this->attacker_properties.block_damage = TURRET_BLOCK_DAMAGE;
        this->attacker_properties.voxel_damage_radius = TURRET_LASER_VOXEL_DAMAGE_RADIUS;
        this->attacker_properties.terrain_modification_action = t_map::TMA_LASER;
        this->target_acquisition_probability = TURRET_TARGET_LOCK_CHANCE;
        this->fire_rate_limit = TURRET_FIRE_LIMIT;

        this->owned.properties.obj = this;
        STATE::owned_list->register_object(&this->owned.properties);

        this->team.properties.obj = this;
        //STATE::team_list->register_object(&this->team.properties);

        this->health.properties.health = TURRET_HEALTH;

        this->voxel_properties.frozen_vox = true;
        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &turret_vox_dat;

        this->spatial.properties.camera_height = TURRET_CAMERA_HEIGHT;
        this->spatial.properties.height = TURRET_HEIGHT;
    }

    ~Turret()
    {
        STATE::owned_list->unregister_object(&this->owned.properties);
    }

    void tick()
    {   // make each a template function
        ObjectState* state = this->state();
        Vec3 position = this->get_position();
        float z = tickStayOnGround(state, position);
        bool changed = this->set_position(position.x, position.y, z);

        this->spatial.properties.set_changed(changed);

        if (this->can_fire())
        {
            this->fire_on_target(
                state->id, state->type, this->get_team(), this->spatial.camera_z(), position,
                state->accuracy_bias, state->sight_range,
                state->attack_enemies, state->attack_random
            );
        }

        if (this->spatial.properties.changed)
            this->broadcastState();
    }

    void update()
    {
        updateFrozenVox(
            this->voxel_properties.vox, this->get_position(),
            this->spatial.properties.angles, this->spatial.properties.changed
        );
        this->spatial.properties.set_changed(false);
    }

    void born(int subtype)
    {
        this->_state.subtype = subtype;
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornTeamVox(this->voxel_properties.vox_dat, state->id, state->type, state->subtype, this->team.properties.team);
        bornSetVox(this->voxel_properties.vox, this->voxel_properties.init_hitscan, this->voxel_properties.init_draw);
        bornUpdateFrozenVox(this->voxel_properties.vox, this->get_position(), this->spatial.properties.angles.x, this->spatial.properties.angles.y);
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        dieExplode(state, this->get_position(), this->get_owner());
        this->broadcastDeath();
        dieRevokeOwner(state->type, this->get_owner());
        if (this->voxel_properties.vox != NULL)
            dieTeamItemAnimation(this->voxel_properties.vox->get_part(0)->get_center(), this->team.properties.team);
        dieChatMessage(this);
    }

};
