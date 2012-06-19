#include "base.hpp"

#include <common/random.h>
#include <physics/vec3.hpp>
#include <agent/agent_physics.hpp>

void Base::set_position(float x, float y, float z)
{
    this->x = translate_point(x);
    this->y = translate_point(y);
    this->z = z;
    this->vox->update(x,y,z, this->theta, this->phi);
}

struct Vec3 Base::get_spawn_point(float agent_height, float agent_radius)
{
    Vec3 spawn;
    
    int x,y;
    x = (int)this->x;
    y = (int)this->y;

    float sx,sy;

    sx = randrange(x-spawn_radius, x+spawn_radius) + 0.5f;
    sy = randrange(y-spawn_radius, y+spawn_radius) + 0.5f;
    sx = translate_point(sx);
    sy = translate_point(sy);
    spawn.x = sx;
    spawn.y = sy;
    spawn.z = _get_highest_open_block(sx, sy, (int)ceil(agent_height));

    while(object_collides_terrain(spawn, agent_height, agent_radius) && spawn.z < map_dim.z)
        spawn.z += 1;

    return spawn;
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
    this->vox = new Voxel_model(&VoxDats::base, id, type, team);
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

