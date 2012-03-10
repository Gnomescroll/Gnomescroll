#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>

extern VoxDat spawner_vox_dat;

const int SPAWNER_RADIUS = 5;
const int SPAWNERS_PER_TEAM = 9;
const int MAX_SPAWNERS = SPAWNERS_PER_TEAM * N_TEAMS;
const int SPAWNER_HEALTH = 300;
const float SPAWNER_HEIGHT = 1.9f;
const int BASE_SPAWN_ID = 255;

class Spawner_create_StoC; // forward decl

class Spawner
{
    private:
        int team;
    public:
        int id;
        int team_index;
        int owner;
        int health;
        Object_types type;

        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int spawn_radius;

        Voxel_model* vox;
        void init_vox();
        int get_team();
        void set_team(int team);
        void set_owner(int owner);

        void set_position(float x, float y, float z);
        
        void get_spawn_point(int agent_height, int* spawn);

        int get_coins_for_kill(int owner, int team);
        int take_damage(int dmg);

        void tick();
        void update(); // client side
        
        #ifdef DC_SERVER
        void create_message(Spawner_create_StoC* msg);
        #endif
        explicit Spawner(int id);
        Spawner(int id, float x, float y, float z);
        ~Spawner();
};

#include <c_lib/template/object_list.hpp>
class Spawner_list: public Object_list<Spawner,MAX_SPAWNERS>
{
    private:
        const char* name() { return "Spawner"; }

    public:
    
        bool team_spawner_available(int team);
        bool point_occupied(int x, int y, int z);
        #ifdef DC_SERVER
        void send_to_client(int client_id);
        #endif
        int get_random_spawner(int team);
        int get_numbered_team_spawner(int team, int id);
        Spawner* get_by_team_index(int team, int team_index);
        bool spawner_exists(int team, int team_index);
        void assign_team_index(Spawner* spawner);

        void tick();
        void update();  // client side

        Spawner_list() { print(); }
};

