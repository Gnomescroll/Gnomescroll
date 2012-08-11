#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <particle/text/billboard_text_hud.hpp>

class Agent_state;

typedef enum
{
    AGENT_VOX_IS_STANDING,
    AGENT_VOX_IS_CROUCHED,
    AGENT_VOX_IS_DEAD,
} AgentVoxStatus;

class Agent_event
{
    private:
        Agent_state* a;
        AgentVoxStatus vox_status;
        bool model_was_changed;
    public:

        bool color_changed;

        class Particle::BillboardTextHud bb;
        void update_hud_name();
        
        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void healed(int health);
        void died();
        void born();
        void crouched();
        void uncrouched();
        bool model_changed();
        void set_agent_vox_status(AgentVoxStatus status);
        void life_changing(bool dead);
        void reload_weapon(int type);

        void name_changed(char* old_name);

        // new style weapon trigger events
        // only triggers agent specific animations/sounds
        void tick_mining_laser();   // continuous, while "on"
        void fired_mining_laser();  // when fire rate tick triggers

        // old style packet handler events
        // still used; triggers target specific animations/sounds
        // since that is required from server
        void fired_weapon_at_object(int id, int type, int part);
        void fired_weapon_at_block(float x, float y, float z, int cube, int side);
        void fired_weapon_at_nothing();
        void melee_attack_object(int id, int type, int part);
        void melee_attack_nothing();
        void fire_empty_weapon(int weapon_type);

        void hit_block();
        void placed_block();
        void threw_grenade();

        void set_spawner(int pt);

        explicit Agent_event(Agent_state* owner);
        ~Agent_event();
};
