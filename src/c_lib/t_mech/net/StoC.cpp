/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "StoC.hpp"

#include <t_mech/common/common.hpp>
#include <t_mech/_interface.hpp>
#include <t_mech/mech_state.hpp>
#include <chat/globals.hpp>

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
        Vec3 p = vec3_scalar_add(vec3_init(m.position), 0.5f);
        SoundID snd_id = Sound::play_3d_sound("cropgrow", p);
        float pmult = float(randrange(-5, 5)) / 1000.0f;
        if (snd_id != NULL_SOUND_ID)
            Sound::set_pitch_multiplier(snd_id, pmult + 1.0f);
    }

    m.type = (MechType)type;
    update_mech(m);
};

void mech_delete_StoC::handle()
{
    //printf("client removing mech %i \n", id);
    mech_list->remove_mech(id);
    //mech_list->needs_update = true;
};


//signs


//uint16_t id;
//char msg[MECH_TEXT_SIZE_MAX+1];
void mech_text_StoC::handle()
{
    if (!Chat::is_valid_chat_message(this->msg)) return;
    for (int i=0; i<MECH_TEXT_SIZE_MAX; i++)
    {   // convert tabs to a space
        if (this->msg[i] == '\t')
            this->msg[i] = ' ';
    }

    msg[MECH_TEXT_SIZE_MAX] = '\0';

    IF_ASSERT(id < 0 || id >= mech_list->mlm) return;
    GS_ASSERT(mech_list->mla[id].id != -1);
    struct Mech& m = mech_list->mla[id];

    class MechAttribute* ma = get_mech_attributes(m.type);
    GS_ASSERT(ma->class_type == MECH_SIGN);
    GS_ASSERT(m.text == NULL);
    m.text = calloc(1, MECH_TEXT_SIZE_MAX+1);
    memcpy(m.text, msg, MECH_TEXT_SIZE_MAX);

    printf("%s: id= %d, text= %s \n", __FUNCTION__, id, (char*)m.text);
};

/*
    for (int i=0; i<CHAT_MESSAGE_SIZE_MAX; i++)
    {   // convert tabs to a space
        if (this->msg[i] == '\t')
            this->msg[i] = ' ';
    }

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
*/

void mech_text_update_StoC::handle()
{
    if (!Chat::is_valid_chat_character(this->key)) return;
    IF_ASSERT(id < 0 || id >= mech_list->mlm) return;
    GS_ASSERT(mech_list->mla[id].id != -1);
    struct Mech& m = mech_list->mla[id];

    IF_ASSERT(m.text == NULL)
        return;
    IF_ASSERT(pos >= MECH_TEXT_SIZE_MAX)
        return;

    class MechAttribute* ma = get_mech_attributes(m.type);
    GS_ASSERT(ma->class_type == MECH_SIGN);

    ((char*)m.text)[pos] = key;

    printf("%s: mech %d, pos %d set to letter %d\n", __FUNCTION__, id, pos, key);
};

#endif


#if DC_SERVER
void mech_create_StoC::handle() {}
void mech_type_change_StoC::handle() {}
void mech_delete_StoC::handle() {}

void mech_text_StoC::handle() {}
void mech_text_update_StoC::handle() {}

#endif

}   // t_mech
