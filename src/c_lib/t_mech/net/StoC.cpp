#include "StoC.hpp"

#include <t_mech/common/common.hpp>
#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>


namespace t_mech
{
    class mech_create_StoC;
    class mech_type_change_StoC;
    class mech_delete_StoC;
}


namespace t_mech
{

#if DC_CLIENT

inline void mech_create_StoC::handle()
{
    //printf("client adding mech node at: %i %i %i \n", x,y,z);
    
    struct MECH m;
    if (t_mech::unpack_mech(m, *this))
        t_mech::mech_list->add_mech(id, m);
    //mech_list->needs_update = true;
};

inline void mech_type_change_StoC::handle()
{
    //printf("client removing mech %i \n", id);
    //mech_list->remove_mech(id);
    //mech_list->needs_update = true;

    GS_ASSERT(mech_list->mla[id].id != -1);
    mech_list->mla[id].mech_type = (MechType) mech_type;
    _mech_update(mech_list->mla[id]);

    //printf("mech %i grow \n", id);

};

//    uint16_t x,y,z;
inline void mech_delete_StoC::handle()
{
    //printf("client removing mech %i \n", id);
    mech_list->remove_mech(id);
    //mech_list->needs_update = true;
};


#endif


#if DC_SERVER

inline void mech_create_StoC::handle() {}
inline void mech_type_change_StoC::handle() {}
inline void mech_delete_StoC::handle() {}

#endif

}   // t_mech
