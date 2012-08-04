#pragma once

class control_node
{
    public:
    
    int x;
    int y;
    int z;
};

class MECH_LIST
{
    public:

    int cpi; //control point index
    int cpm; //control point max
    class MECH* cpa; //control point array;

    bool needs_update; //for drawing

    MECH_LIST()
    {
        cpi = 0;
        cpm = 8;
        cpa = (MECH*) malloc(8*sizeof(class MECH));
        needs_update = true;
    }

    ~MECH_LIST()
    {
        free(cpa);
    }

    void add_mech(int x, int y, int z)
    {
        //needs_update = true;

        cpa[cpi].x = x;
        cpa[cpi].y = y;
        cpa[cpi].z = z;

        cpi++;

        if(cpi == cpm)
        {
            cpm *= 2;
            MECH* new_cpa = (MECH*) realloc(cpa, cpm*sizeof(class MECH));
            if (new_cpa == NULL)
            {
                free(cpa);
                cpa = NULL;
                cpm = 0;
            }
            else cpa = new_cpa;
        }
    }

    void remove_mech(int x, int y, int z)
    {
        //needs_update = true;

        for(int i=0; i<cpi; i++)
        {
            if(x==cpa[i].x && y==cpa[i].y && z==cpa[i].z)
            {
                cpa[i] = cpa[cpi-1];
                cpi--;
                GS_ASSERT(cpi >= 0);
            }
        }

        printf("Error: tried to remove control point that does not exist!\n");
        GS_ABORT(); //should never reach this point
    }


#if DC_SERVER

    void send_mech_list_to_client(int client_id)
    {
        for(int i=0; i<cpi; i++)
        {
            control_node_create_StoC p;
            p.x = cpa[i].x;
            p.y = cpa[i].y;
            p.z = cpa[i].z;
            p.sendToClient(client_id);
        }
    }

    void server_add_mech(int x, int y, int z)
    {
        return; //PRODUCTION
        this->add_mech(x,y,z);

        control_node_create_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

    void server_remove_mech(int x, int y, int z)
    {
        remove_mech(x,y,z);

        control_node_delete_StoC p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.broadcast();
    }

#endif

};





}

