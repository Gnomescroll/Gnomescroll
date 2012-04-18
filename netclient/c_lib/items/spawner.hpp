#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/common/api/include.hpp>
#include <c_lib/entity/network/interfaces.hpp>
#if DC_CLIENT
#include <c_lib/sound/sound.hpp>
#endif

//forward decl
class SpawnerList;

#if DC_SERVER
namespace ServerState
{
    extern SpawnerList* spawner_list;
    extern OwnedList* owned_list;
}
#endif
#if DC_CLIENT
namespace ClientState
{
    extern SpawnerList* spawner_list;
    extern OwnedList* owned_list;
}
#endif

extern VoxDat spawner_vox_dat;

/* Spawner Component */

class SpawnerProperties: public ComponentProperties
{
    public:
    int radius;
    
    SpawnerProperties()
    : radius(1)
    {}
};

class SpawnerComponent
{
    public:
        SpawnerProperties spawner_properties;
        void get_spawn_point(Vec3 position, int spawned_object_height, int* spawn_pt);
    SpawnerComponent(){}
};

const int SPAWNER_LIST_MAX = 256;
class SpawnerList: public BehaviourList
{
    private:
        const char* name() { return "SpawnerList"; }
    public:
    int max_per_team;

    bool team_spawner_available(int team);
    int get_random_spawner(int team);
    int get_numbered_team_spawner(int team, int id);
    ObjectPolicyInterface* get_by_team_index(int team, int team_index);
    bool spawner_exists(int team, int team_index);
    void assign_team_index(ObjectPolicyInterface* spawner);

    SpawnerList()
    : BehaviourList(SPAWNER_LIST_MAX), max_per_team(SPAWNERS_PER_TEAM)
    {}
};

class Spawner: public SpawnerComponent, public VoxelComponent, public ObjectStateLayer
{
    public:
        OwnedComponent owned;
        TeamComponent team;
        HealthComponent health;
        PositionChangedComponent spatial;
    
    explicit Spawner(int id)
    : SpawnerComponent(), ObjectStateLayer(Objects::create_packet_owner_team_index, Objects::state_packet, &owned, &team, &health, &spatial)
    {
        this->_state.id = id;
        this->_state.cost = COST_SPAWNER;
        this->_state.reward = COST_SPAWNER;
        this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
        this->_state.type = OBJECT_SPAWNER;
        
        this->health.properties.health = SPAWNER_HEALTH;

        this->spawner_properties.obj = this;
        this->spawner_properties.radius = SPAWNER_SPAWN_RADIUS;
        STATE::spawner_list->register_object(&this->spawner_properties);

        this->owned.properties.obj = this;
        STATE::owned_list->register_object(&this->owned.properties);

        this->team.properties.obj = this;
        //STATE::team_list->register_object(&this->team.properties);

        this->voxel_properties.frozen_vox = true;
        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &spawner_vox_dat;

        this->spatial.properties.height = SPAWNER_HEIGHT;
    }

    ~Spawner()
    {
        STATE::spawner_list->unregister_object(&this->spawner_properties);
        STATE::owned_list->unregister_object(&this->owned.properties);
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

    void born()
    {
        ObjectState* state = this->state();
        Vec3 position = this->get_position();
        this->voxel_properties.vox = bornTeamVox(this->voxel_properties.vox_dat, state->id, state->type, this->team.properties.team);
        bornSetVox(this->voxel_properties.vox, this->voxel_properties.init_hitscan, this->voxel_properties.init_draw);
        bornUpdateFrozenVox(this->voxel_properties.vox, position, this->spatial.properties.angles.x, this->spatial.properties.angles.y);

        #if DC_SERVER
        this->broadcastCreate();
        #endif

        #if DC_CLIENT
        Sound::spawner_placed(position.x, position.y, position.z, 0,0,0); // TODO -- generic sound handler
        #endif
    }

    void die()
    {
        ObjectState* state = this->state();
        this->broadcastDeath();
        dieRevokeOwner(state->type, this->get_owner());
        if (this->voxel_properties.vox != NULL)
            dieTeamItemAnimation(this->voxel_properties.vox->get_part(0)->get_center(), this->team.properties.team);
        dieChatMessage(this);
    }
};

