#pragma once

#include <entity/constants.hpp>
#include <agent/constants.hpp>
#include <common/color.hpp>
#include <social/badges.hpp>

namespace Agents
{

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
class AgentStatus
{
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

        size_t n_badges;
        BadgeType badges[PLAYER_MAX_BADGES];

        bool vox_crouched;

        int lifetime;

        Color color;

    void tick();

    void set_name(const char* name);

    void set_spawner(int pt);

    bool die();
    bool die(AgentID inflictor_id, EntityType inflictor_type, AgentDeathMethod death_method);
    void kill(int victim_id);
    void kill_slime();

    void quit();

    bool set_color(Color color);
    bool set_color_silent(Color color);

    float get_spawn_angle();

    void add_badge(BadgeType badge_id);

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

    void send_scores(ClientID client_id);
    void send_scores();

    void send_badges(ClientID client_id);
    void broadcast_badges();
    #endif

    explicit AgentStatus(class Agent* a);
    ~AgentStatus();

    private:
        class Agent* a;
        int voxel_model_restore_throttle;
};

}   // Agents
