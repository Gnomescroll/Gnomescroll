#pragma once


//#include <t_mech/net/CtoS.hpp>
#if DC_SERVER
#include <t_mech/net/StoC.hpp>
#endif

namespace t_mech 
{

const int MECH_HARD_MAX = 0xff;

static void pack_mech(struct MECH &m, class mech_create_StoC &p);
static void unpack_mech(struct MECH &m, class mech_create_StoC &p);

class MECH_LIST
{
    public:

    int mli; //mech index
    int mlm; //mech max
    int mln; //number of mech

    class MECH* mla; //mech array;

    bool needs_update; //for drawing

    MECH_LIST()
    {
        mli = 0;
        mlm = 8;
        mln = 0;
        mla = (MECH*) malloc(8*sizeof(class MECH));
        for(int i=0; i<mlm; i++) mla[i].id = -1;

        needs_update = true;
    }

    ~MECH_LIST()
    {
        free(mla);
    }

    bool is_occupied(int x, int y, int z); //check if there is a t_mech on the square already
    
#if DC_SERVER
    void handle_block_removal(int x, int y, int z);
#endif

#if DC_CLIENT
    void add_mech(int id, const struct MECH &m)
    {
        while(id >= mlm)
        {
            printf("add_mech: expand array id= %i mlm= %i from %i \n", id, 2*mlm, mlm);
            mla = (class MECH*) realloc(mla, 2*mlm*sizeof(class MECH));
            for(int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        GS_ASSERT(mla[id].id == -1);
        GS_ASSERT(id < MECH_HARD_MAX);

        mla[id] = m; //store mech
        mla[id].id = id;
        mln++;
    
        printf("create mech %i at: %i %i %i \n", m.id, m.x,m.y,m.z);
    }

#endif

#if DC_SERVER
    //negative 1 on failure
    int add_mech(struct MECH &m)
    {
        //needs_update = true;
        if(mlm == MECH_HARD_MAX) return -1; //test max creation limit (set to 0xffff)

        if(mln == mlm)
        {
            mla = (MECH*) realloc(mla, 2*mlm*sizeof(class MECH));
            for(int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        int _mli = mli;
        while(mla[mli].id != -1)
        {
            mli = (mli + 1) % mlm;
            GS_ASSERT(mli != _mli);
        }

        m.id = mli;   //set id
        mla[mli] = m; //store mech
        mln++;

        return mli;
    }
#endif

    void remove_mech(int id)
    {
        GS_ASSERT(mla[id].id != -1);
        GS_ASSERT(id < mlm);


        if( mla[id].id == -1)
            printf("Error t_mech::remove_mech, tried to remove mech that does not exist!\n");

        mla[id].id = -1;
        mln--;
        GS_ASSERT(mln >= 0);


    }


#if DC_SERVER
    void send_mech_list_to_client(int client_id)
    {
        for(int i=0; i<mlm; i++)
        {
            if(mla[i].id == -1) continue;
            mech_create_StoC p;
            pack_mech(mla[i], p);
            p.sendToClient(client_id);
        }
    }

    void server_add_mech(struct MECH &m)
    {
        this->add_mech(m);

        mech_create_StoC p;
        pack_mech(m, p);
        p.broadcast();
    }

    void server_remove_mech(int id)
    {
        this->remove_mech(id);
        mech_delete_StoC p;
        p.id = id;
        p.broadcast();
    }

#endif

};


bool MECH_LIST::is_occupied(int x, int y, int z)
{
    for(int i=0; i<mlm; i++)
    {
        if( mla[i].id == -1) continue;
        if( mla[i].x == x && mla[i].y == y && mla[i].z == z) return true;
    }
    return false;
}

#if DC_SERVER
void MECH_LIST::handle_block_removal(int x, int y, int z)
{
    for(int i=0; i<mlm; i++)
    {
        if( mla[i].id == -1) continue;
        if( mla[i].x == x && mla[i].y == y && mla[i].z == z+1)
        {
            server_remove_mech(i);
            return;
        }
    }
}
#endif

}

