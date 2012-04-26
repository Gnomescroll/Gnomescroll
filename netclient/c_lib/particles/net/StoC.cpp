#include "StoC.hpp"

#if DC_CLIENT
#include <c_lib/t_item/_interface.hpp>
#include <c_lib/t_hud/_interface.hpp>
#endif

namespace Particles
{


#if DC_CLIENT

inline void item_particle_create_StoC::handle()
{
    printf("item_particle: creating %d \n", id);
    // create item via list
    
    //Free_item* f = t_item::item_particle_list->create(id);
    //f->init(x,y,z,mx,my,mz);

    //Animations::spawn_insect_mob(x,y,z);
}

inline void item_particle_destroy_StoC::handle()
{
    printf("item_particle: destroying %d \n", id);
    // destroy item via list
    //t_item::item_particle_list->destroy(id);
}


inline void item_particle_picked_up_StoC::handle()
{
    printf("item_particle: %d picked up by agent %d \n", id, agent_id);
    /*
        Item Pickup Sound
    */

   // t_item::item_particle_list->destroy(id);
}

#endif

#if DC_SERVER

inline void item_particle_create_StoC::handle() {}
inline void item_particle_destroy_StoC::handle() {}
inline void item_particle_picked_up_StoC::handle() {}

#endif

}