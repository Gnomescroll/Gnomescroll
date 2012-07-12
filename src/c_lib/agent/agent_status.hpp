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
        class Agent_state* a;
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

        bool vox_crouched;

        int lifetime;

        class Inventory* inventory;
        class Inventory* toolbelt;

        void tick();

        bool set_name(char* n); // return true if the new name is different
        void check_missing_name();

        void set_spawner(int pt);
        void set_spawner();

        void send_health_msg();
        void heal(unsigned int amt);
        int apply_damage(int dmg);
        int apply_damage(int dmg, int inflictor_id, ObjectType inflictor_type, int part_id=-1);
        int apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, ObjectType inflictor_type);
        bool die();
        bool die(int inflictor_id, ObjectType inflictor_type, AgentDeathMethod death_method);
        void kill(int victim_id);
        void kill_slime();
        #if DC_SERVER
        void respawn();
        void set_fresh_state();
        #endif
        void restore_health();
        void at_base();
        
        void send_scores(int client_id);
        void send_scores();


        float get_spawn_angle();

        // item stuff
        const bool can_gain_item(ObjectType item);
        bool gain_item(int item_id, ObjectType item);
        #if DC_SERVER
        bool consume_item(ItemID item_id);
        #endif
        bool lose_item(ObjectType item);

        explicit Agent_status(Agent_state* a);
        ~Agent_status();
};
