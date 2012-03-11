#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>

extern VoxDat turret_vox_dat;

const int TURRETS_PER_PLAYER = 10;
//const int MAX_TURRETS = TURRETS_PER_PLAYER * AGENT_MAX;
const int MAX_TURRETS = 256;
const int TURRET_HEALTH = 125;
const float TURRET_HEIGHT = 1.9f;
const float TURRET_CAMERA_HEIGHT = 1.6f;
const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;    // same as agent
const int TURRET_FIRE_LIMIT = 30; // once per second
const int TURRET_BLOCK_DAMAGE = 8;
//const int TURRET_AGENT_DAMAGE = 15;
const int TURRET_AGENT_DAMAGE = 10;

class turret_create_StoC; // forward decl

class Turret
{
    private:
        unsigned int fire_limiter;
        int team;
    public:
        int id;
        int owner;
        int health;
        Object_types type;

        float x,y,z;
        float theta,phi;
        float camera_height; // z offset to fire from
        
        Voxel_model* vox;
        void init_vox();
        int get_team();
        void set_team(int team);
        void set_owner(int owner);

        void set_position(float x, float y, float z);
        
        int get_coins_for_kill(int owner, int team);
        int take_damage(int dmg);
        void acquire_target();

        void tick();
        void update(); // client side
        
        #ifdef DC_SERVER
        void create_message(turret_create_StoC* msg);
        #endif
        explicit Turret(int id);
        Turret(int id, float x, float y, float z);
        ~Turret();
};

#include <c_lib/template/object_list.hpp>
class Turret_list: public Object_list<Turret,MAX_TURRETS>
{
    private:
        const char* name() { return "Turret"; }

    public:
    
        bool point_occupied(int x, int y, int z);
        #ifdef DC_SERVER
        void send_to_client(int client_id);
        #endif

        void tick();
        void update();  // client side

        Turret_list() { print(); }
};

