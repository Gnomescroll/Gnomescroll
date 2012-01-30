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

Base::Base(int id, int team)
:
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
