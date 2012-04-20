#include "flag.hpp"

VoxDat flag_vox_dat;

void Flag::set_position(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(x,y,z, this->theta, this->phi);
}

Flag::Flag(int id, int team)
:
x(0), y(0), z(0),
theta(0), phi(0),
id(id), team(team),
type(OBJECT_FLAG),
held(false)
{
    this->vox = new Voxel_model(&flag_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #if DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
}

Flag::~Flag()
{
    if (this->vox != NULL)
        delete this->vox;
}

void Flag::tick()
{
#if DC_SERVER
    float old_z = this->z;

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
    if (old_z != (float)z)
        ServerState::ctf->set_flag_position(this->team, this->x, this->y, (float)z);
#endif
}

void Flag::animate()
{
    if (this->vox == NULL) return;

    float dtheta = 0.02f;
    this->theta += dtheta;
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
}

void Flag::update()
{
    #if DC_CLIENT
    this->vox->was_updated = false;
    Vec3 center = this->vox->get_part(0)->get_center();
    float radius = this->vox->get_part(0)->radius;
    if (sphere_fulstrum_test(center.x, center.y, center.z, radius) == false)
        this->vox->set_draw(false);
    else
        this->vox->set_draw(true);
    if (input_state.skeleton_editor)
        this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
    #endif
}
