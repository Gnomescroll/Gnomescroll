#include "StoC.hpp"

#include <t_mech/common/common.hpp>
#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>

// fuck msvc
namespace t_mech
{

#if DC_CLIENT
void mech_create_StoC::handle()
{
    //printf("client adding mech node at: %i %i %i \n", x,y,z);

    struct Mech m;
    if (unpack_mech(m, *this))
        mech_list->add_mech(id, m);
    //mech_list->needs_update = true;
};

void mech_type_change_StoC::handle()
{
    //printf("client removing mech %i \n", id);
    //mech_list->remove_mech(id);
    //mech_list->needs_update = true;

    IF_ASSERT(id < 0 || id >= mech_list->mlm) return;

    GS_ASSERT(mech_list->mla[id].id != -1);
    struct Mech& m = mech_list->mla[id];

    if (is_plant(m.type))
    {
        int snd_id = Sound::play_3d_sound("cropgrow", vec3_init(m.x, m.y, m.z));
        float pmult = float(randrange(-5,5)) / 1000.0f;
        if (snd_id >= 0)
            Sound::set_pitch_multiplier(snd_id, pmult+1.0f);
    }

    m.type = (MechType)type;
    _mech_update(m);
};

void mech_delete_StoC::handle()
{
    //printf("client removing mech %i \n", id);
    mech_list->remove_mech(id);
    //mech_list->needs_update = true;
};
#endif


#if DC_SERVER
void mech_create_StoC::handle() {}
void mech_type_change_StoC::handle() {}
void mech_delete_StoC::handle() {}
#endif

}   // t_mech
