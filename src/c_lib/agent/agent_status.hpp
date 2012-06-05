#pragma once

#include <entity/constants.hpp>
#include <agent/constants.hpp>

void switch_agent_ownership(int item_id, ObjectType item_type, int owner, int new_owner);

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
//class Agent_status: public Base_status {
class Agent_status {

    private:
        Agent_state* a;
        int voxel_model_restore_throttle;

    public:

        int health;
        bool dead;
        int respawn_countdown;
        int spawner;

        unsigned int kills;
        unsigned int deaths;
        unsigned int suicides;
        unsigned int slime_kills;

        unsigned int health_max;

        int owned_spawners;
        int owned_turrets;

        char name[PLAYER_NAME_MAX_LENGTH + 1];
        bool identified;

        int team;
        bool has_flag;
        unsigned int flag_captures;

        bool vox_crouched;

        int base_restore_rate_limiter;

        int lifetime;

        class Inventory* inventory;
        class Inventory* toolbelt;

        void tick();

        bool set_name(char* n); // return true if the new name is different
        void check_missing_name();

        void set_spawner(int pt);
        void set_spawner();

        void heal(unsigned int amt);
        int apply_damage(int dmg);
        int apply_damage(int dmg, int inflictor_id, ObjectType inflictor_type, int part_id=-1);
        int apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, ObjectType inflictor_type);
        int die();
        int die(int inflictor_id, ObjectType inflictor_type, AgentDeathMethod death_method);
        void kill(int victim_id);
        void kill_slime();
        void respawn();
        void restore_health();
        void at_base();
        
        bool pickup_flag();
        bool drop_flag();
        void score_flag();

        int score();
        void send_scores(int client_id);
        void send_scores();

        void set_team(int team);

        float get_spawn_angle();
        void check_if_at_base();

        // item stuff
        const bool can_gain_item(ObjectType item);
        bool gain_item(int item_id, ObjectType item);
        #if DC_SERVER
        bool gain_stack_item(ObjectType type, int id, int stack_size);
        #endif
        bool lose_item(ObjectType item);

        explicit Agent_status(Agent_state* a);
        ~Agent_status();
};
