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

        struct Mech* mla; //mech array;

        bool needs_update; //for drawing

    MechList() :
        mli(0), mlm(1024), mln(0), needs_update(true)
    {
        this->mla = (Mech*)malloc(this->mlm*sizeof(struct Mech));
        for (int i=0; i<mlm; i++)
        {
            mla[i].id = -1;
            mla[i].type = NULL_MECH_TYPE;
        }
    }

    ~MechList()
    {
        free(mla);
    }

    struct Mech* get(int mech_id)
    {
        if (mech_id < 0 || mech_id >= this->mlm)
            return NULL;
        if (this->mla[mech_id].id == -1)
            return NULL;
        return &this->mla[mech_id];
    }

    bool is_occupied(const Vec3i& position); //check if there is a t_mech on the square already

    #if DC_SERVER
    MechType handle_block_removal(Vec3i position);
    #endif

    #if DC_CLIENT
    void add_mech(int id, const struct Mech &m)
    {
        GS_ASSERT(mln < MECH_HARD_MAX);

        GS_ASSERT(m.text == NULL);

        while (id >= mlm)
        {
            //printf("add_mech: expand array id= %i mlm= %i from %i \n", id, 2*mlm, mlm);
            mla = (struct Mech*)realloc(mla, 2*mlm*sizeof(struct Mech));
            for (int i=mlm; i<2*mlm; i++)
            {
                mla[i].id = -1;
                mla[i].type = NULL_MECH_TYPE;
            }
            mlm *= 2;
        }

        GS_ASSERT(mla[id].id == -1);
        GS_ASSERT(id < MECH_HARD_MAX);

        mla[id] = m; //store mech
        mla[id].id = id;
        mln++;

        //printf("create mech %i type %d at: %i %i %i \n", m.id, m.type, m.x,m.y,m.z);
    }
    #endif

    #if DC_SERVER
    //negative 1 on failure
    int add_mech(struct Mech &m)
    {
        IF_ASSERT(mln < 0 || mln >= MECH_HARD_MAX) return -1;

        if (mln >= mlm)
        {
            mla = (struct Mech*) realloc(mla, 2*mlm*sizeof(struct Mech));
            for (int i=mlm; i<2*mlm; i++)
            {
                mla[i].id = -1;
                mla[i].type = NULL_MECH_TYPE;
            }
            mlm *= 2;
        }

        int _mli = mli;
        int i = 0;
        for (; i<mlm; i++)
        {
            _mli = (i+mli)%mlm;
            if (mla[_mli].id == -1) break;
        }
        IF_ASSERT(i == mlm) return -1;
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

        if(mla[id].text != NULL)
            free(mla[id].text);

        mla[id].id = -1;
        mln--;
        GS_ASSERT(mln >= 0);
        return true;
    }

    int count(MechType type)
    {
        int count = 0;
        for (int i=0; i<this->mlm; i++)
            if (this->mla[i].type == type)
                count++;
        return count;
    }

    #if DC_SERVER
    void send_mech_list_to_client(ClientID client_id);
    bool server_add_mech(struct Mech &m);
    bool server_remove_mech(int id);
    #endif
};

}   // t_mech
