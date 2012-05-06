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
    create_item_particle(id, item_type, x,y,z,mx,my,mz);
}

inline void item_particle_destroy_StoC::handle()
{
    destroy(id);
}

inline void item_particle_state_StoC::handle()
{
    ItemParticle* particle = item_particle_list->get(id);
    if (particle == NULL) return;
    particle->set_state(x,y,z,mx,my,mz);
}

inline void item_particle_picked_up_StoC::handle()
{
    destroy(id);
    Sound::pickup_item();
}
#endif

#if DC_SERVER
inline void item_particle_create_StoC::handle() {}
inline void item_particle_destroy_StoC::handle() {}
inline void item_particle_state_StoC::handle() {}
inline void item_particle_picked_up_StoC::handle() {}
#endif

}
