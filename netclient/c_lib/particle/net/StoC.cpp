#include "StoC.hpp"

#if DC_CLIENT
#include <particle/_interface.hpp>
#include <item/_interface.hpp>
#include <item/data/constant.hpp>
#include <sound/sound.hpp>
#endif

namespace Particle
{

#if DC_CLIENT
inline void item_particle_create_StoC::handle()
{
    Particle::create_item_particle(id, item_type, x,y,z,mx,my,mz);
}

inline void item_particle_destroy_StoC::handle()
{
    Particle::destroy(id);
}

inline void item_particle_picked_up_StoC::handle()
{
    Particle::destroy(id);
    Sound::pickup_item();
}
#endif

#if DC_SERVER
inline void item_particle_create_StoC::handle() {}
inline void item_particle_destroy_StoC::handle() {}
inline void item_particle_picked_up_StoC::handle() {}
#endif

}
