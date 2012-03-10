//#pragma once

//#include <c_lib/voxel/voxel_model.hpp>
//#include <c_lib/voxel/voxel_dat.hpp>
//#include <c_lib/common/enum_types.hpp>

//extern VoxDat turret_vox_dat;

//const int TURRETS_PER_PLAYER = 10;
//const int MAX_TURRETS = TURRETS_PER_PLAYER * MAX_PLAYERS;
//const int TURRET_HEALTH = 200;
//const float TURRET_HEIGHT = 1.9f;

//class Turret_create_StoC; // forward decl

//class Turret
//{
    //private:
        //int team;
    //public:
        //int id;
        //int owner;
        //int health;
        //Object_types type;

        //float x,y,z;
        ////float vx,vy,vz;
        //float theta,phi;

        //Voxel_model* vox;
        //void init_vox();
        //int get_team();
        //void set_team(int team);
        //void set_owner(int owner);

        //void set_position(float x, float y, float z);
        
        //void get_spawn_point(int agent_height, int* spawn);

        //int get_coins_for_kill(int owner, int team);
        //int take_damage(int dmg);

        //void tick();
        //void update(); // client side
        
        //#ifdef DC_SERVER
        //void create_message(Turret_create_StoC* msg);
        //#endif
        //explicit Turret(int id);
        //Turret(int id, float x, float y, float z);
        //~Turret();
//};

//#include <c_lib/template/object_list.hpp>
//class Turret_list: public Object_list<Turret,MAX_TURRETS>
//{
    //private:
        //const char* name() { return "Turret"; }

    //public:
    
        //bool team_turret_available(int team);
        //bool point_occupied(int x, int y, int z);
        //#ifdef DC_SERVER
        //void send_to_client(int client_id);
        //#endif
        //int get_random_turret(int team);
        //int get_numbered_team_turret(int team, int id);
        //Turret* get_by_team_index(int team, int team_index);
        //bool turret_exists(int team, int team_index);
        //void assign_team_index(Turret* turret);

        //void tick();
        //void update();  // client side

        //Turret_list() { print(); }
//};

