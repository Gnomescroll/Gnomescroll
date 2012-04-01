#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

extern VoxDat spawner_vox_dat;

//class Spawner
//{
    //private:
        //int team;
        //int owner;
    //public:
        //int id;
        //unsigned int team_index;
        //int health;
        //Object_types type;

        //float x,y,z;
        //float theta,phi;

        //int spawn_radius;

        //Voxel_model* vox;
        //void init_vox();
        //int get_team();
        //void set_team(int team);
        //int get_owner();
        //void set_owner(int owner);

        //void set_position(float x, float y, float z);
        
        //void get_spawn_point(int agent_height, int* spawn);

        //int get_coins_for_kill(int owner, int team);
        //int take_damage(int dmg);

        //void tick();
        //void update(); // client side
        
        //#ifdef DC_SERVER
        //void create_message(object_create_owner_team_index_StoC* msg);
        //#endif
        //explicit Spawner(int id);
        //Spawner(int id, float x, float y, float z);
        //~Spawner();
//};

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


//#include <c_lib/template/object_list.hpp>
//class Spawner_list: public Object_list<Spawner,MAX_SPAWNERS>
//{
    //private:
        //const char* name() { return "Spawner"; }

    //public:
    
        //bool team_spawner_available(int team);
        //bool point_occupied(int x, int y, int z);
        //#ifdef DC_SERVER
        //void send_to_client(int client_id);
        //#endif
        //int get_random_spawner(int team);
        //int get_numbered_team_spawner(int team, int id);
        //Spawner* get_by_team_index(int team, int team_index);
        //bool spawner_exists(int team, int team_index);
        //void assign_team_index(Spawner* spawner);

        ////void tick();
        ////void update();  // client side

        ////void alter_owner(int owner, int new_owner);

        //Spawner_list() { print(); }
//};

