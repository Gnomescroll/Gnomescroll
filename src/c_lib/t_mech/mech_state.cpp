/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "mech_state.hpp"

namespace t_mech
{

bool MechList::is_occupied(const Vec3i& position)
{
    for (int i=0; i<mlm; i++)
        if (mla[i].id >= 0 && is_equal(mla[i].position, position))
            return true;
    return false;
}

#if DC_SERVER
MechType MechList::handle_block_removal(Vec3i position)
{
    position.z += 1;    // we're comparing against mechs one z level above
    MechType type = NULL_MECH_TYPE;
    for (int i=0; i<mlm; i++)
        if (mla[i].id != -1 && is_equal(mla[i].position, position))
        {
            type = mla[i].type;
            bool removed = server_remove_mech(i);
            GS_ASSERT(removed);
        }
    return type;
}

void MechList::send_mech_list_to_client(ClientID client_id)
{
    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;
        class mech_create_StoC p;
        pack_mech(mla[i], p);
        p.sendToClient(client_id);

        if (mla[i].text != NULL)
        {
            class mech_text_StoC p;
            p.id = mla[i].id;
            memcpy(p.msg, mla[i].text, MECH_TEXT_SIZE_MAX);
            p.sendToClient(client_id);
        }
    }

}

bool MechList::server_add_mech(struct Mech &m)
{
    IF_ASSERT(mln >= MECH_HARD_MAX)
    {
        printf("MechList::server_add_mech error: t_mech limit reached \n");
        return false;
    }

    this->add_mech(m);

    class mech_create_StoC p;
    pack_mech(m, p);
    p.broadcast();

    if (m.text != NULL)
    {
        class mech_text_StoC p;
        p.id = m.id;
        memcpy(p.msg, m.text, MECH_TEXT_SIZE_MAX);
        p.broadcast();
    }

    return true;
}

bool MechList::server_remove_mech(int id)
{
    bool removed = this->remove_mech(id);
    if (removed)
    {
        class mech_delete_StoC p;
        p.id = id;
        p.broadcast();
    }
    return removed;
}

#endif

}   // t_mech
