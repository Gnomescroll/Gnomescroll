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
    create_item_particle(id, item_type, x,y,z,mx,my,mz);
}

inline void item_particle_destroy_StoC::handle()
{
    GS_ASSERT(id != NULL_PARTICLE);
    if (id == NULL_PARTICLE) return;
    
	// if particle destroyed while targeting agent (i.e. it is being
	// picked up by agent) then assume agent picked it up
    ItemParticle* particle = get(id);
    GS_ASSERT(particle != NULL);
    if (particle == NULL) return;
    if (particle->target_agent != NO_AGENT
     && particle->target_agent == ClientState::playerAgent_state.agent_id)
    {
        static int pickup_sound_frame = 0;
        if (pickup_sound_frame != ClientState::frame_id)
        {
            pickup_sound_frame = ClientState::frame_id;
            Sound::pickup_item();
        }
	}
	
    destroy(id);
}

inline void item_particle_state_StoC::handle()
{
    ItemParticle* particle = get(id);
    if (particle == NULL) return;
    ASSERT_BOXED_POINT(x);
    ASSERT_BOXED_POINT(y);
    particle->set_state(x,y,z,mx,my,mz);
}

inline void item_particle_picked_up_StoC::handle()
{
	GS_ASSERT(id != NULL_PARTICLE);
	if (id == NULL_PARTICLE) return;
	ItemParticle* particle = get(id);
	GS_ASSERT(particle != NULL);
	if (particle == NULL) return;
	
	particle->picked_up(agent_id);
}

inline void item_particle_pickup_cancelled_StoC::handle()
{
	GS_ASSERT(id != NULL_PARTICLE);
	if (id == NULL_PARTICLE) return;
	ItemParticle* particle = get(id);
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

}	// ItemParticle
