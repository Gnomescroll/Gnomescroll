#include "mech_state.hpp"

namespace t_mech
{
    
bool MechList::is_occupied(int x, int y, int z)
{
    for(int i=0; i<mlm; i++)
        if (mla[i].id >= 0 && mla[i].x == x && mla[i].y == y && mla[i].z == z)
            return true;
    return false;
}

#if DC_SERVER
MechType MechList::handle_block_removal(int x, int y, int z)
{
    MechType mech_type = NULL_MECH_TYPE;
    for (int i=0; i<mlm; i++)
        if (mla[i].id != -1 && mla[i].x == x && mla[i].y == y && mla[i].z == z+1)
        {
            mech_type = mla[i].mech_type;
            bool removed = server_remove_mech(i);
            GS_ASSERT(removed);
        }
    return mech_type;
}

void MechList::send_mech_list_to_client(ClientID client_id)
{
    for (int i=0; i<mlm; i++)
    {
        if (mla[i].id == -1) continue;
        class mech_create_StoC p;
        pack_mech(mla[i], p);
        p.sendToClient(client_id);
    }
}

void MechList::server_add_mech(struct MECH &m)
{
    IF_ASSERT(mln >= MECH_HARD_MAX)
    {
        printf("MechList::server_add_mech error: t_mech limit reached \n");
        return;
    }

    this->add_mech(m);

    class mech_create_StoC p;
    pack_mech(m, p);
    p.broadcast();
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
