#pragma once

#include <entity/constants.hpp>
#include <agent/constants.hpp>
#include <common/color.hpp>

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
//class AgentStatus: public Base_status {
class AgentStatus {

    private:
        class Agent* a;
        int voxel_model_restore_throttle;

    public:

        int health;
        bool should_die;
        bool dead;
        int respawn_countdown;
        int spawner;

        unsigned int kills;
        unsigned int deaths;
        unsigned int suicides;
        unsigned int slime_kills;

        unsigned int health_max;

        char name[PLAYER_NAME_MAX_LENGTH + 1];

        bool vox_crouched;

        int lifetime;
        
        struct Color color;

        void tick();

        void set_name(const char* name);

        //#if DC_CLIENT
        //void check_missing_name();
        //#endif

        void set_spawner(int pt);

        bool die();
        bool die(AgentID inflictor_id, EntityType inflictor_type, AgentDeathMethod death_method);
        void kill(int victim_id);
        void kill_slime();

        void quit();

        #if DC_SERVER

        void identify(const char* name);    // returns false if malformed
        
        void respawn();
        void set_fresh_state();

        void send_health_msg();
        void send_health_msg(ClientID client_id);
        void restore_health();
        void heal(unsigned int amt);
        int apply_damage(int dmg);
        int apply_damage(int dmg, AgentID inflictor_id, EntityType inflictor_type, int part_id=-1);
        int apply_hitscan_laser_damage_to_part(int part_id, AgentID inflictor_id, EntityType inflictor_type);
        int apply_mining_laser_damage_to_part(int part_id, AgentID inflictor_id, EntityType inflictor_type);
        void at_base();

        void send_color(ClientID client_id);
        void broadcast_color();

        bool consume_item(ItemID item_id);

        #endif
        
        void send_scores(ClientID client_id);
        void send_scores();

        bool set_color(struct Color color);
        bool set_color_silent(struct Color color);

        float get_spawn_angle();

        explicit AgentStatus(Agent* a);
        ~AgentStatus();
};
