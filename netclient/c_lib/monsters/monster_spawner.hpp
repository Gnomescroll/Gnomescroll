#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/common/api/include.hpp>
#include <c_lib/objects/common/net/interfaces.hpp>

namespace Monsters
{

extern VoxDat monster_spawner_vox_dat;

/* MonsterSpawner Component */

class MonsterSpawnerComponent
{
    public:
        Object_types type;
        float radius;
        void get_spawn_point(Vec3 position, float spawned_object_height, float* spawn_pt);
    MonsterSpawnerComponent(){}
};

class MonsterSpawner: public MonsterSpawnerComponent, public VoxelComponent, public ObjectStateLayer
{
    public:
        HealthComponent health;
        PositionChangedComponent spatial;

        MonsterSpawnerComponent spawn;
    
    explicit MonsterSpawner(int id)
    : MonsterSpawnerComponent(), ObjectStateLayer(Objects::create_packet, Objects::state_packet, Objects::owned_none, Objects::team_none, &health, &spatial)
    {
        this->_state.id = id;
        this->_state.reward = MONSTER_SPAWNER_REWARD;
        this->_state.coin_rule = COINS_ANYONE;
        
        this->health.properties.health = MONSTER_SPAWNER_HEALTH;

        this->spawn.radius = MONSTER_SPAWNER_SPAWN_RADIUS;

        this->voxel_properties.frozen_vox = true;
        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &monster_spawner_vox_dat;

        this->spatial.properties.height = MONSTER_SPAWNER_HEIGHT;
    }

    ~MonsterSpawner()
    {
    }

    void tick()
    {
        ObjectState* state = this->state();
        Vec3 position = this->get_position();
        float z = tickStayOnGround(state, position);
        bool changed = this->set_position(position.x, position.y, z);
        this->spatial.properties.set_changed(changed);

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

    void draw() {}

    void born()
    {
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type);
        bornSetVox(this->voxel_properties.vox, this->voxel_properties.init_hitscan, this->voxel_properties.init_draw);
        bornUpdateFrozenVox(this->voxel_properties.vox, this->get_position(), this->spatial.properties.angles.x, this->spatial.properties.angles.y);
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        this->broadcastDeath();
        dieRevokeOwner(state->type, this->get_owner());
        //if (this->voxel_properties.vox != NULL)
            //dieTeamItemAnimation(this->voxel_properties.vox->get_part(0)->get_center(), this->team.properties.team);
        dieChatMessage(this);
    }
};

}
