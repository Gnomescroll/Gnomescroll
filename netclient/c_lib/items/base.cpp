#include "base.hpp"

#include <c_lib/common/random.h>

VoxDat base_vox_dat;

void Base::set_position(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(x,y,z, this->theta, this->phi);
}

void Base::get_spawn_point(int agent_height, Vec3* spawn)
{
    int x,y;
    x = (int)this->x;
    y = (int)this->y;

    int sx,sy;

    sx = randrange(x-spawn_radius, x+spawn_radius);
    sx = (sx < map_dim.x) ? sx : map_dim.x;
    sx = (sx < 0) ? 0 : sx;

    sy = randrange(y-spawn_radius, y+spawn_radius);
    sy = (sy < map_dim.y) ? sy : map_dim.y;
    sy = (sy < 0) ? 0 : sy;

    spawn->x = sx;
    spawn->y = sy;
    spawn->z = _get_highest_open_block(sx, sy, agent_height);
}

void Base::tick()
{
#if DC_SERVER
    float old_z = this->z;

    int x,y,z;
    x = (int)this->x;
    y = (int)this->y;
    z = (int)this->z;
    if (isSolid(_get(x,y,z)))
    {   // move up
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
    {   // fall down
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

    if (old_z != (float)z)  // sends packet
        ServerState::ctf->set_base_position(this->team, this->x, this->y, (float)z);
#endif
}

void Base::update()
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

Base::Base(int id, int team)
:
x(0), y(0), z(0),
theta(0), phi(0),
id(id), team(team),
type(OBJECT_BASE),
spawn_radius(BASE_SPAWN_RADIUS)
{
    this->vox = new Voxel_model(&base_vox_dat, id, type, team);
    this->vox->set_hitscan(false);
    #if DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
}

Base::~Base()
{
    if (this->vox != NULL)
        delete this->vox;
}

