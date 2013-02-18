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
    GS_ASSERT(id != NULL_PARTICLE);
    if (id == NULL_PARTICLE) return;
    ASSERT_BOXED_POINT(x);
    ASSERT_BOXED_POINT(y);
    create_item_particle((ItemParticleID)id, (ItemType)item_type, x,y,z,mx,my,mz);
}

inline void item_particle_destroy_StoC::handle()
{
    GS_ASSERT(id != NULL_PARTICLE);
    if (id == NULL_PARTICLE) return;

    // if particle destroyed while targeting agent (i.e. it is being
    // picked up by agent) then assume agent picked it up
    ItemParticle* particle = get((ItemParticleID)id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return;
    if (particle->target_agent != NULL_AGENT
     && particle->target_agent == ClientState::player_agent.agent_id)
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
    ASSERT_BOXED_POINT(x);
    ASSERT_BOXED_POINT(y);
    particle->set_state(x,y,z,mx,my,mz);
}

inline void item_particle_picked_up_StoC::handle()
{
    GS_ASSERT(id != NULL_PARTICLE);
    if (id == NULL_PARTICLE) return;
    ItemParticle* particle = get((ItemParticleID)id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return;

    particle->picked_up((AgentID)this->agent_id);
}

inline void item_particle_pickup_cancelled_StoC::handle()
{
    GS_ASSERT(id != NULL_PARTICLE);
    if (id == NULL_PARTICLE) return;
    ItemParticle* particle = get((ItemParticleID)id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return;
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
