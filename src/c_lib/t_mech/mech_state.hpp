#pragma once


//#include <t_mech/net/CtoS.hpp>
#if DC_SERVER
#include <t_mech/net/StoC.hpp>
#endif

namespace t_mech 
{

const int MECH_HARD_MAX = 0xff;

class MECH
{
    public:
    int id; //-1 if not in use

    int x;
    int y;
    int z;

    float radius;
    int type;
    int offset;
    int rotation;
};

enum MECH_TYPE
{
	MECH_CRYSTAL,
	MECH_CROP,
	MECH_WIRE,
	MECH_SWITCH
};

class MECH_LIST
{
    public:

    int mli; //mech index
    int mlm; //mech max
    int mln; //number of mech
    int 
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

#if DC_CLIENT
    void add_mech(int id, struct MECH &m)
    {
        while(id >= mlm)
        {
            mla = (MECH*) realloc(mla, 2*mlm*sizeof(class MECH));
            for(int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        GS_ASSERT(mla[id].id == -1);
        GS_ASSERT(id < MECH_HARD_MAX);

        m.id = id;
        mla[mli] = m; //store mech
        mln++;
    }

#endif

#if DC_SERVER
    //returns id
    int add_mech(struct MECH &m)
    {
        //needs_update = true;
        if(mlm == MECH_HARD_MAX) return -1; //test max creation limit (set to 0xffff)

        is(mln == mlm)
        {
            mla = (MECH*) realloc(mla, 2*mlm*sizeof(class MECH));
            for(int i=mlm; i<2*mlm; i++) mla[i].id = -1;
            mlm *= 2;
        }

        int _mli = mli;
        while(mla[mli].index != -1)
        {
            mli = (mli + 1) % mlm;
            GS_ASSERT(mli != _mli);
        }

        m.id = mli;
        mla[mli] = m; //store mech
        mln++;

        return mli; //return id
    }
#endif

    void remove_mech(int id)
    {
        GS_ASSERT(mla[id].id != -1);
        GS_ASSERT(id < mlm);
        mla[id].id = -1;
        mln--;
        GS_ASSERT(mln >= 0);

        printf("Error: tried to remove mech that does not exist!\n");
    }


#if DC_SERVER

/*
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_WIRE,
    MECH_SWITCH
*/
    //pack mech data into packet
    static void pack_mech(const struct MECH &m, class mech_create_StoC &p)
    {
        p.id = m.id;
        p.x = m.x;
        p.y = m.y;
        p.z = m.z;

        switch ( m.type )
        {
        case MECH_CRYSTAL:
            //do something
            break;
        default:
            printf("pack_mech error: unhandled mech type\n");
        }
    }

    //handles unpacking
    static void unpack_mech(const struct MECH &m, class mech_create_StoC &p)
    {
        m.id = p.id;
        m.x = p.x;
        m.y = p.y;
        m.z = p.z;

        switch ( p.type )
        {
        case MECH_CRYSTAL:
            //do something
            break;
        default:
            printf("pack_mech error: unhandled mech type\n");
        } 
    }

    void send_mech_list_to_client(int client_id)
    {
        for(int i=0; i<mlm; i++)
        {
            if(mla[i],id == -1) continue;
            pack_mech(mla[i], p);
            p.sendToClient(client_id);
        }
    }

    void server_add_mech(struct MECH &m)
    {
        this->add_mech(m);

        mech_create_StoC p;
        pack_mech(mla[i], p);
        p.broadcast();
    }

    void server_remove_mech(int id)
    {
        remove_mech(x,y,z,0);

        mech_delete_StoC p;
        p.id = id;
        p.broadcast();
    }

#endif

};





}

