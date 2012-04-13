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
        int max_children;
        int children;

        void get_spawn_point(Vec3 position, float spawned_object_height, float* spawn_pt);
        bool full() { return (this->children >= this->max_children); }
        void gain_child(Object_types type, int id);
        void lose_child(Object_types type, int id);
        
    MonsterSpawnerComponent()
    : type(OBJ_TYPE_NONE), radius(1.0f), max_children(1), children(0)
    {}
};

class MonsterSpawner:
    public VoxelComponent,
    #if DC_CLIENT
    public AnimationVoxelComponent,
    #endif
    public ObjectStateLayer
{
    public:
        HealthComponent health;
        PositionChangedComponent spatial;

        #if DC_SERVER
        MonsterSpawnerComponent spawn;
        #endif
    
    explicit MonsterSpawner(int id)
    : ObjectStateLayer(Objects::create_packet, Objects::state_packet, Objects::owned_none, Objects::team_none, &health, &spatial)
    {
        this->_state.id = id;
        this->_state.reward = MONSTER_SPAWNER_REWARD;
        this->_state.coin_rule = COINS_ANYONE;
        
        this->health.properties.health = MONSTER_SPAWNER_HEALTH;

        #if DC_SERVER
        this->spawn.radius = MONSTER_SPAWNER_SPAWN_RADIUS;
        this->spawn.max_children = MONSTER_SPAWNER_MAX_CHILDREN;
        #endif
        
        this->voxel_properties.frozen_vox = true;
        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &monster_spawner_vox_dat;

        this->spatial.properties.height = MONSTER_SPAWNER_HEIGHT;

        #if DC_CLIENT
        this->animation_size = MONSTER_SPAWNER_ANIMATION_PARTICLE_SIZE;
        this->animation_count = MONSTER_SPAWNER_ANIMATION_PARTICLE_COUNT;
        this->animation_color = MONSTER_SPAWNER_ANIMATION_COLOR;
        #endif
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

    void born(int subtype)
    {
        this->_state.subtype = subtype;
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type, state->subtype);
        bornSetVox(this->voxel_properties.vox, this->voxel_properties.init_hitscan, this->voxel_properties.init_draw);
        bornUpdateFrozenVox(this->voxel_properties.vox, this->get_position(), this->spatial.properties.angles.x, this->spatial.properties.angles.y);
        this->broadcastCreate();
    }

    void die()
    {
        #if DC_SERVER
        this->broadcastDeath();
        #endif

        #if DC_CLIENT
        //dieChatMessage(this);
        Vec3 position = this->get_center(MONSTER_SPAWNER_PART_BODY);
        this->animation_voxel_explode(position);
        #endif
    }
};

}
