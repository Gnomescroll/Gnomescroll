#pragma once

#if DC_SERVER
# include <t_mech/net/StoC.hpp>
#endif

#include <t_mech/_interface.hpp>

namespace t_mech 
{

const int MECH_HARD_MAX = 0XFFFF-1;

struct MechList
{
    public:

        int mli; //mech index
        int mlm; //mech max
        int mln; //number of mech

        //const static HARD_MAX = 0xffff-1;

        struct MECH* mla; //mech array;

        bool needs_update; //for drawing

    MechList()
    {
        mli = 0;
        mlm = 8;
        mln = 0;
        mla = (MECH*)malloc(8*sizeof(struct MECH));
        for(int i=0; i<mlm; i++) mla[i].id = -1;

        needs_update = true;
    }

    ~MechList()
    {
        free(mla);
    }

    bool is_occupied(int x, int y, int z); //check if there is a t_mech on the square already
    
    #if DC_SERVER
    MechType handle_block_removal(int x, int y, int z);
    #endif

    #if DC_CLIENT
    void add_mech(int id, const struct MECH &m)
    {
        GS_ASSERT(mln < MECH_HARD_MAX);

        while (id >= mlm)
        {
            //printf("add_mech: expand array id= %i mlm= %i from %i \n", id, 2*mlm, mlm);
            mla = (struct MECH*)realloc(mla, 2*mlm*sizeof(struct MECH));
            for (int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        GS_ASSERT(mla[id].id == -1);
        GS_ASSERT(id < MECH_HARD_MAX);

        mla[id] = m; //store mech
        mla[id].id = id;
        mln++;
    
        //printf("create mech %i type %d at: %i %i %i \n", m.id, m.mech_type, m.x,m.y,m.z);
    }
    #endif

    #if DC_SERVER
    //negative 1 on failure
    int add_mech(struct MECH &m)
    {
        GS_ASSERT(mln >= 0 && mln < MECH_HARD_MAX);
        if(mln >= MECH_HARD_MAX) return -1; //test max creation limit (set to 0xffff)

        if(mln >= mlm)
        {
            mla = (struct MECH*) realloc(mla, 2*mlm*sizeof(struct MECH));
            for(int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        int _mli = mli;
        int i = 0;
        for (; i<mlm; i++)
        {
            _mli = (i+mli)%mlm;
            if (mla[_mli].id == -1) break;
        }
        GS_ASSERT(i != mlm);
        if (i == mlm) return -1;
        mli = _mli;
        
        m.id = mli;   //set id
        mla[mli] = m; //store mech
        mln++;

        return mli;
    }
    #endif

    bool remove_mech(int id)
    {
        IF_ASSERT(id < 0 || id >= this->mlm) return false;
        IF_ASSERT(mla[id].id == -1) return false;

        mla[id].id = -1;
        mln--;
        GS_ASSERT(mln >= 0);
        return true;
    }

    #if DC_SERVER
    void send_mech_list_to_client(ClientID client_id);
    void server_add_mech(struct MECH &m);
    bool server_remove_mech(int id);
    #endif
};

}   // t_mech
