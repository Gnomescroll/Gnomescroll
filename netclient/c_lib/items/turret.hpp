#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/voxel/voxel_dat.hpp>
#include <c_lib/common/enum_types.hpp>
#include <c_lib/behaviour/behaviour.hpp>
#include <c_lib/entity/entity.hpp>

// forward decl
class object_shot_object_StoC;
class object_shot_terrain_StoC;
class object_shot_nothing_StoC;

void turret_shot_object(object_shot_object_StoC* msg);
void turret_shot_terrain(object_shot_terrain_StoC* msg);
void turret_shot_nothing(object_shot_nothing_StoC* msg);

extern VoxDat turret_vox_dat;

//class Turret
//{
    //private:
        //unsigned int fire_limiter;
        //int team;
        //int owner;
    //public:
        //int id;
        //int health;
        //Object_types type;

        //float x,y,z;
        //float theta,phi;
        //float camera_height; // z offset to fire from
        
        //Voxel_model* vox;
        //void init_vox();
        //int get_team();
        //void set_team(int team);
        //int get_owner();
        //void set_owner(int owner);
        
        //void set_position(float x, float y, float z);
        
        //int get_coins_for_kill(int owner, int team);    // REMAINING
        //int take_damage(int dmg);
        //void acquire_target();

        //void tick();
        //void update(); // client side
        
        //#ifdef DC_SERVER
        //void create_message(object_create_owner_team_StoC* msg);   // REMAINING
        //#endif
        //explicit Turret(int id);
        //Turret(int id, float x, float y, float z);
        //~Turret();
//};

/* TargetAcquistion component */

class TargetAcquisitionComponent
{
    public:
    void acquire_target(ObjectState* state);
    TargetAcquisitionComponent(){}
};

class Turret; // forward decl

typedef TickStayOnGround < TickTargetAcquisition < NoTick(Turret), Turret>,Turret>
    TurretTick;

typedef UpdateFrozenVox < NoUpdate(Turret), Turret> TurretUpdate;

typedef BornTeamVox < BornSetVox < BornUpdateFrozenVox < BornCreateOwnerTeamMessage < NoBorn(Turret), Turret>,Turret>,Turret>,Turret>
    TurretBorn;

typedef DieExplode < DieBroadcast < DieRevokeOwner < DieTeamItemAnimation < NoDie(Turret), Turret>,Turret>,Turret>,Turret> TurretDie;

typedef ObjectPolicy
<Turret, TurretTick, NoDraw(Turret), TurretUpdate, TurretBorn, TurretDie, create_object_owner_team_message, object_state_message >
TurretObjectParent;

class Turret: public TurretObjectParent, public TargetAcquisitionComponent
{
    public:
        Turret(int id, float x, float y, float z, float mx, float my, float mz)
        : TurretObjectParent(this), TargetAcquisitionComponent()
        {
            this->_state.id = id;
            this->_state.position = vec3_init(x,y,z);
            this->_state.camera_height = TURRET_CAMERA_HEIGHT;
            this->_state.broadcast_state_change = true;
            this->_state.blow_up_on_death = true;
            this->_state.explosion_radius = TURRET_EXPLOSION_RADIUS;
            this->_state.explosion_damage = TURRET_EXPLOSION_DAMAGE;
            this->_state.cost = COST_TURRET;
            this->_state.reward = COST_TURRET;
            this->_state.coin_rule = COINS_ENEMIES | COINS_OWNER;
            this->_state.fire_rate_limit = TURRET_FIRE_LIMIT;
            this->_state.sight_range = TURRET_SIGHT_RANGE;
            this->_state.accuracy_bias = TURRET_LASER_BIAS;
            this->_state.target_acquisition_probability = TURRET_TARGET_LOCK_CHANCE;
            this->_state.attack_enemies = false;
            this->_state.attack_random = true;
            this->_state.suicidal = false;
            this->_state.frozen_vox = true;
            this->_state.vox_dat = &turret_vox_dat;
            this->_state.type = OBJ_TYPE_TURRET;
            this->_state.health = TURRET_HEALTH;
            this->_state.attacker_properties.agent_protection_duration = AGENT_TURRET_PROTECTION_DURATION;
            this->_state.attacker_properties.agent_damage = TURRET_AGENT_DAMAGE;
            this->_state.attacker_properties.block_damage = TURRET_BLOCK_DAMAGE;
            this->_state.attacker_properties.voxel_damage_radius = TURRET_LASER_VOXEL_DAMAGE_RADIUS;
            this->_state.attacker_properties.terrain_modification_action = t_map::TMA_LASER;
        }
};

//#include <c_lib/template/object_list.hpp>
//class Turret_list: public Object_list<Turret,MAX_TURRETS>
//{
    //private:
        //const char* name() { return "Turret"; }

    //public:
    
        //bool point_occupied(int x, int y, int z);
        //#ifdef DC_SERVER
        //void send_to_client(int client_id);
        //#endif

        //void tick();
        //void update();  // client side
        //void alter_owner(int owner, int new_owner);

        //Turret_list() { print(); }
//};

