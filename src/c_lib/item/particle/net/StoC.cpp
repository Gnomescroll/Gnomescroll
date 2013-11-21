/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "StoC.hpp"

#if DC_CLIENT
#include <item/particle/_interface.hpp>
#include <item/_interface.hpp>
#include <sound/sound.hpp>
#endif

namespace ItemParticle
{

#if DC_CLIENT
inline void item_particle_create_StoC::handle()
{
    IF_ASSERT(id == NULL_PARTICLE) return;
    create_item_particle((ItemParticleID)id, (ItemType)item_type,
                         this->position, this->velocity);
}

inline void item_particle_destroy_StoC::handle()
{
    IF_ASSERT(id == NULL_PARTICLE) return;
    // if particle destroyed while targeting agent (i.e. it is being
    // picked up by agent) then assume agent picked it up
    ItemParticle* particle = get((ItemParticleID)id);
    IF_ASSERT(particle == NULL) return;
    if (particle->target_agent != NULL_AGENT &&
        particle->target_agent == ClientState::player_agent.agent_id)
    {
        static int pickup_sound_frame = 0;
        if (pickup_sound_frame != ClientState::frame_id)
        {
            pickup_sound_frame = ClientState::frame_id;
            Sound::play_2d_sound("pickup_item");
        }
    }

    destroy((ItemParticleID)id);
}

inline void item_particle_state_StoC::handle()
{
    ItemParticle* particle = get((ItemParticleID)id);
    if (particle == NULL) return;
    GS_ASSERT(is_boxed_position(this->position));
    particle->set_state(this->position, this->velocity);
}

inline void item_particle_picked_up_StoC::handle()
{
    IF_ASSERT(id == NULL_PARTICLE) return;
    ItemParticle* particle = get((ItemParticleID)id);
    IF_ASSERT(particle == NULL) return;
    particle->picked_up((AgentID)this->agent_id);
}

inline void item_particle_pickup_cancelled_StoC::handle()
{
    IF_ASSERT(id == NULL_PARTICLE) return;
    ItemParticle* particle = get((ItemParticleID)id);
    IF_ASSERT(particle == NULL) return;
    particle->pickup_cancelled();
}

#endif

#if DC_SERVER
inline void item_particle_create_StoC::handle() {}
inline void item_particle_destroy_StoC::handle() {}
inline void item_particle_state_StoC::handle() {}
inline void item_particle_picked_up_StoC::handle() {}
inline void item_particle_pickup_cancelled_StoC::handle() {}
#endif

}   // ItemParticle
