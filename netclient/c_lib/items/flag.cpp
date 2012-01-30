#include "flag.hpp"

VoxDat flag_vox_dat;

void Flag::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&flag_vox_dat, x,y,z, this->theta, this->phi);
}

Flag::Flag(int id, int team)
:
x(0), y(0), z(0),
theta(0), phi(0),
id(id), team(team),
type(OBJ_TYPE_FLAG)
{
    this->vox = new Voxel_model(&flag_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Flag::~Flag()
{
    if (this->vox != NULL) delete this->vox;
}

void Flag::tick()
{
#ifdef DC_SERVER
    int x,y,z;
    x = (int)this->x;
    y = (int)this->y;
    z = (int)this->z;
    if (isSolid(_get(x,y,z)))
    {
        // move up
        while (isSolid(_get(x,y,++z)))
        {
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
        }
    }
    else
    {
        // fall down
        while (!isSolid(_get(x,y,--z)))
        {
            if (z<=0)
            {
                z = 0;
                break;
            }
        }
        z++;
    }
    ServerState::ctf.set_flag_position(this->team, this->x, this->y, (float)z);
#endif
}


//#include <net_lib/net.hpp>

//class flag_state_StoC: public FixedSizeReliableNetPacketToClient<flag_state_StoC>
//{
    //public:
        //int team;
        //float x,y,z;
        ////float vx,vy,vz;

        //inline void packet(char* buff, int* buff_n, bool pack) 
        //{
            //pack_u8(&id, buff, buff_n, pack);
            //pack_float(&x, buff, buff_n, pack);
            //pack_float(&y, buff, buff_n, pack);
            //pack_float(&z, buff, buff_n, pack);
        //}
        //inline void handle();
//};


//#ifdef DC_CLIENT
//inline void flag_state_StoC::handle()
//{
    //ClientState::ctf.set_flag_position(team,x,y,z);
//}
//#endif

//#ifdef DC_SERVER
//inline void flag_state_StoC::handle(){}
//#endif

