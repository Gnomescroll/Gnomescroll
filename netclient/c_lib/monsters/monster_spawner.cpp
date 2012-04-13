#include "monster_spawner.hpp"

#include <math.h>
#include <c_lib/common/random.h>
#include <c_lib/chat/client.hpp>
#include <net_lib/net.hpp>
#include <c_lib/common/quicksort.hpp>

namespace Monsters
{

/* Packets */

#ifdef DC_CLIENT

void monster_spawner_create(object_create_owner_team_index_StoC_model* msg)
{
    MonsterSpawner* s = (MonsterSpawner*)ClientState::object_list->create((Object_types)msg->type, (int)msg->id);
    if (s == NULL)
    {
        printf("WARNING monster_spawner_create() -- could not create monster_spawner %d\n", msg->id);
        return;
    }
    s->set_position(msg->x, msg->y, msg->z);
    s->set_team(msg->team);
    s->set_team_index((unsigned int)msg->team_index); //overwrite with server authority
    s->set_owner(msg->owner);
    s->born(0); // TODO
    //Sound::monster_spawner_placed(msg->x, msg->y, msg->z, 0,0,0);
    system_message->object_created(s);
}
#endif

/* MonsterSpawners */

VoxDat monster_spawner_vox_dat;

/* MonsterSpawnerComponent */

void MonsterSpawnerComponent::get_spawn_point(Vec3 position, float spawned_object_height, float* spawn_pt)
{
    float sx,sy;
    sx = position.x + ((randf() * this->radius * 2) - this->radius);
    sx = (sx > map_dim.x - 1) ? map_dim.x - 1 : sx;
    sx = (sx < 0) ? 0 : sx;

    sy = position.y + ((randf() * this->radius * 2) - this->radius);
    sy = (sy < map_dim.y - 1) ? sy : map_dim.y - 1;
    sy = (sy < 0) ? 0 : sy;

    int h = (int)ceil(spawned_object_height);
    spawn_pt[2] = t_map::get_highest_open_block((int)sx, (int)sy, h);
    spawn_pt[0] = sx;
    spawn_pt[1] = sy;
}

void MonsterSpawnerComponent::gain_child(Object_types type, int id)
{
    this->children++;
}

void MonsterSpawnerComponent::lose_child(Object_types type, int id)
{
    this->children--;
}


// can assume max 2 splits -- 1 per orthogonal side set (ie top or bottom && left or right)

//// return n lines; fills in lines buffer
//int split_lines(float a, float b, float x, float y, float lines[3][4])
//{
    //// assume source in bounds?
    //bool oobx = (x > map_dim.x || x < 0);
    //bool ooby = (y > map_dim.y || y < 0);

    //if (oobx || ooby)
    //{
        //float xd = x-map_dim.x/2;
        //float yd = y-map_dim.y/2;
        //if (abs(xd) > abs(yd))   // x closer to edge (distance from middle is greater
        //{
            //// split on x line,
                //// then, from this point, split on y line
            ////lines[0][0] = a;
            ////lines[0][1] = b;
            ////lines[0][2] = a+xd;
            ////lines[0][3] = y;

            //// fix x to appropriate side (0 or map_dim.x)
            //// calculate y as intersection point (simple linear)

            //// if y, do same for that
            
        //}
        //else    // y closer to edge
        //{

        //}

        //if (oobx && ooby) return 3;
        //return 2;
    //}
    //else
    //{
        //lines[0][0] = a;
        //lines[0][1] = b;
        //lines[0][2] = x;
        //lines[0][3] = y;
        //return 1;
    //}
//}

} // Monsters
