#include "StoC.hpp"

#include <t_mech/_interface.hpp>

#include <t_mech/mech_state.hpp>

namespace t_mech
{

#if DC_CLIENT


//    uint16_t x,y,z;
void mech_create_StoC::handle()
{
    printf("client adding mech node at: %i %i %i \n", x,y,z);
    mech_list->add_mech(x,y,z,0);
    //mech_list->needs_update = true;
};

//    uint16_t x,y,z;
void mech_delete_StoC::handle()
{
    printf("client removing mech node at: %i %i %i \n", x,y,z);
    mech_list->remove_mech(x,y,z,0);
    //mech_list->needs_update = true;
};


#endif


#if DC_SERVER

void mech_create_StoC::handle() {}
void mech_delete_StoC::handle() {}

#endif

}   // t_map