#include "base.hpp"

#include <c_lib/common/random.h>

VoxDat base_vox_dat;

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&base_vox_dat, x,y,z, this->theta, this->phi);
}

void Base::get_spawn_point(int *spawn, int agent_height)
{
    int x,y;
    x = (int)this->x;
    y = (int)this->y;

    int sx,sy;
    sx = randrange(x-spawn_radius, x+spawn_radius);
    sy = randrange(y-spawn_radius, y+spawn_radius);
    spawn[0] = (sx < map_dim.x) ? sx : map_dim.x;
    spawn[0] = (spawn[0] < 0) ? 0 : spawn[0];
    spawn[1] = (sy < map_dim.y) ? sy : map_dim.y;
    spawn[1] = (spawn[1] < 0) ? 0 : spawn[1];
    spawn[2] = _get_highest_open_block(spawn[0], spawn[1], agent_height);
}

void Base::tick()
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
    ServerState::ctf.set_base_position(this->team, this->x, this->y, (float)z);
#endif
}

Base::Base(int id, int team)
:
x(0), y(0), z(0),
theta(0), phi(0),
id(id), team(team),
type(OBJ_TYPE_BASE),
spawn_radius(BASE_SPAWN_RADIUS)
{
    this->vox = new Voxel_model(&base_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::~Base()
{
    if (this->vox != NULL) delete this->vox;
}


//#include <net_lib/net.hpp>

//class base_state_StoC: public FixedSizeReliableNetPacketToClient<base_state_StoC>
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
//inline void base_state_StoC::handle()
//{
    //ClientState::ctf.set_base_position(team,x,y,z);
//}
//#endif

//#ifdef DC_SERVER
//inline void base_state_StoC::handle(){}
//#endif

