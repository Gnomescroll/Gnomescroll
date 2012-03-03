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
    if (this->vox != NULL)
        delete this->vox;
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
    ServerState::ctf->set_flag_position(this->team, this->x, this->y, (float)z);
#endif
}

void Flag::animate()
{
    if (this->vox == NULL) return;

    float dtheta = 0.02f;
    this->theta += dtheta;
    this->vox->update(&flag_vox_dat, this->x, this->y, this->z, this->theta, this->phi);
}

void Flag::update()
{
    #ifdef DC_CLIENT
    if (current_camera == NULL || !current_camera->in_view(x,y,z))
    {
        this->vox->set_draw(false);
        this->vox->set_draw(false);
    }
    else
    {
        this->vox->set_draw(true);
        this->vox->set_draw(true);
    }
    #endif
}
