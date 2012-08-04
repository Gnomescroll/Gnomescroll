#pragma once


//#include <t_mech/net/CtoS.hpp>
#if DC_SERVER
#include <t_mech/net/StoC.hpp>
#endif

namespace t_mech 
{


class MECH
{
    public:
    int id;

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
	CRYSTAL,
	CROP,
	WIRE,
	SWITCH
};

class MECH_LIST
{
    public:

    int mli; //control point index
    int mlm; //control point max
    class MECH* mla; //control point array;

    bool needs_update; //for drawing

    MECH_LIST()
    {
        mli = 0;
        mlm = 8;
        mla = (MECH*) malloc(8*sizeof(class MECH));
        needs_update = true;
    }

    ~MECH_LIST()
    {
        free(mla);
    }

    //accept in an MLA struct!!
    void add_mech(int x, int y, int z, int type)
    {
        //needs_update = true;

        mla[mli].x = x;
        mla[mli].y = y;
        mla[mli].z = z;

    	mla[mli].radius = 1.0;
    	mla[mli].offset = 0;
    	type = 0;	//Crystal;

        mli++;

        if(mli == mlm)
        {
            mlm *= 2;
            MECH* new_mla = (MECH*) realloc(mla, mlm*sizeof(class MECH));
            if (new_mla == NULL)
            {
                free(mla);
                mla = NULL;
                mlm = 0;
            }
            else mla = new_mla;
        }
    }

    void remove_mech(int x, int y, int z, int type)
    {
        //needs_update = true;

        for(int i=0; i<mli; i++)
        {
            if(x==mla[i].x && y==mla[i].y && z==mla[i].z && type==mla[i].type)
            {
                mla[i] = mla[mli-1];
                mli--;
                GS_ASSERT(mli >= 0);
            }
        }

        printf("Error: tried to remove control point that does not exist!\n");
        GS_ABORT(); //should never reach this point
    }


#if DC_SERVER

    void send_mech_list_to_client(int client_id)
    {
        for(int i=0; i<mli; i++)
        {
            mech_create_StoC p;
            p.x = mla[i].x;
            p.y = mla[i].y;
            p.z = mla[i].z;
            p.sendToClient(client_id);
        }
    }

    void server_add_mech(int x, int y, int z)
    {
        return; //PRODUCTION
        this->add_mech(x,y,z,0);

        mech_create_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

    void server_remove_mech(int x, int y, int z)
    {
        remove_mech(x,y,z,0);

        mech_delete_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

#endif

};





}

