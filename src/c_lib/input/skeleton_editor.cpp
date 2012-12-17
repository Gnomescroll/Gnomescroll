#include "skeleton_editor.hpp"

#include <input/handlers.hpp>
#include <time.h>

namespace SkeletonEditor
{
bool rotate = false;
bool use_skeleton = false;
EntityType type = OBJECT_AGENT;
int id = 0;
int part = 0;
VoxDat* vox_dat = NULL;
Voxel_model* vox = NULL;
void *obj = NULL;
VoxPart* vp = NULL;
int node = 0;

float lateral_speed = 0.05f;
const int ROT_STEPS = 16;   // key presses for a full cycle
float rotation_speed = 2.0f / ROT_STEPS;

void move_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->sx += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void move_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->sy += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void move_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->sz += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void rotate_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->srx += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void rotate_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->sry += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void rotate_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->srz += s;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

void reset_part()
{
    if (vox_dat == NULL || vox == NULL || vp == NULL) return;
    vp->sx = 0;
    vp->sy = 0;
    vp->sz = 0;
    vp->srx = 0;
    vp->sry = 0;
    vp->srz = 0;
    vp->set_local_matrix();    // TODO: use node
    vox->set_vox_dat(vox_dat);
    vox->reset_skeleton();   
}

/*
// use this macro in a case label to get correct type cast
#define GET_VOX_STUFF(TYPE) \
obj = ClientState::entity_list->get(type, id); \
if (obj==NULL) return; \
vox = ((TYPE*)obj)->voxel_properties.vox; \
vox_dat = ((TYPE*)obj)->voxel_properties.vox_dat; \
break;
*/

void raycast_to_part()
{
    // get camera vector
    Vec3 vec = current_camera->forward_vector();
    Vec3 p = current_camera->get_position();

    // hitscan against voxels
    float vox_distance = 10000000.0f;
    float collision_point[3];
    Voxel_hitscan_target target;
    bool voxel_hit = ClientState::voxel_hitscan_list->hitscan(
        p.x,p.y,p.z,
        vec.x,vec.y,vec.z,
        -1, (EntityType)-1,
        collision_point, &vox_distance,
        &target
    );

    if (!voxel_hit)
        return;

    id = target.entity_id;
    part = target.part_id;
    type = (EntityType)target.entity_type;

    VoxDat* old = vox_dat;
    switch (type)
    {
        case OBJECT_AGENT:
            vox_dat = &VoxDats::agent;
            obj = Agents::get_agent((AgentID)id);
            GS_ASSERT(obj != NULL);
            if (obj == NULL) return;
            vox = ((Agent*)obj)->vox;
            break;

        //case OBJECT_AGENT_SPAWNER:
            //GET_VOX_STUFF(Spawner)
        //case OBJECT_TURRET:
            //GET_VOX_STUFF(Turret)
        //case OBJECT_MONSTER_BOMB:
            //GET_VOX_STUFF(Monsters::Slime)
        //case OBJECT_MONSTER_BOX:
            //GET_VOX_STUFF(Monsters::Box)

        default:
            printf("Skeleton_editor -- raycast_to_part() -- unhandled obj type %d\n", type);
            return;
    }
    if (old != vox_dat)
        node = 0;
    vp = vox_dat->vox_part[part];
}

//#undef GET_VOX_STUFF

/* Skeleton nodes */

void reset_node()
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][0] = 0;
    vox_dat->vox_skeleton_local_matrix_reference[node][1] = 0;
    vox_dat->vox_skeleton_local_matrix_reference[node][2] = 0;
    vox_dat->vox_skeleton_local_matrix_reference[node][3] = 0;
    vox_dat->vox_skeleton_local_matrix_reference[node][4] = 0;
    vox_dat->vox_skeleton_local_matrix_reference[node][5] = 0;
    vox_dat->reset_skeleton_local_matrix(node);
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void move_node_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][0] += s;
    vox_dat->reset_skeleton_local_matrix(node);
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void move_node_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][1] += s;
    vox_dat->reset_skeleton_local_matrix(node);
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void move_node_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][2] += s;
    vox_dat->reset_skeleton_local_matrix(node);  
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void rotate_node_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][3] += s;
    vox_dat->reset_skeleton_local_matrix(node);
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void rotate_node_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][4] += s;
    vox_dat->reset_skeleton_local_matrix(node); 
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

void rotate_node_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    vox_dat->vox_skeleton_local_matrix_reference[node][5] += s;
    vox_dat->reset_skeleton_local_matrix(node); 
    if (node == 0)
        vox->set_skeleton_root(vox_dat->vox_skeleton_local_matrix_reference[node]);
    else
    {
        vox->set_vox_dat(vox_dat);
        vox->reset_skeleton();
    }
}

/* Handlers */
    
void key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_ESCAPE:
        case SDLK_o:
            toggle_skeleton_editor();
            break;

        case SDLK_r:
            rotate = (!rotate);
            break;

        case SDLK_t:
            use_skeleton = (!use_skeleton);
            break;

        case SDLK_k:
            reset_node();
            break;
        case SDLK_l:
            reset_part();
            break;

        case SDLK_RETURN:
            save();
            break;
        
        case SDLK_LEFT:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_x(rotation_speed);
                else
                    rotate_in_x(rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_x(lateral_speed);
                else
                    move_in_x(lateral_speed);
            break;
        case SDLK_RIGHT:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_x(-rotation_speed);
                else
                    rotate_in_x(-rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_x(-lateral_speed);
                else
                    move_in_x(-lateral_speed);
            break;
        case SDLK_UP:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_y(rotation_speed);
                else
                    rotate_in_y(rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_y(lateral_speed);
                else
                    move_in_y(lateral_speed);
            break;
        case SDLK_DOWN:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_y(-rotation_speed);
                else
                    rotate_in_y(-rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_y(-lateral_speed);
                else
                    move_in_y(-lateral_speed);
            break;
        case SDLK_PERIOD:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_z(rotation_speed);
                else
                    rotate_in_z(rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_z(lateral_speed);
                else
                    move_in_z(lateral_speed);
            break;
        case SDLK_COMMA:
            if (rotate)
                if (use_skeleton)
                    rotate_node_in_z(-rotation_speed);
                else
                    rotate_in_z(-rotation_speed);
            else
                if (use_skeleton)
                    move_node_in_z(-lateral_speed);
                else
                    move_in_z(-lateral_speed);
            break;

        case SDLK_RIGHTBRACKET: // toggle thru parts
            if (vox_dat == NULL || vox == NULL) break;
            part++;
            part %= vox->n_parts;
            vp = vox_dat->vox_part[part];
            break;

        case SDLK_EQUALS:
        case SDLK_PLUS:
            if (vox_dat == NULL || vox == NULL) break;
            node++;
            node %= vox->n_skeleton_nodes;
            printf("Selected node %d\n", node);
            break;
            
        default: break;
    }
}

void key_up_handler(SDL_Event* event)
{}

void mouse_button_down_handler(SDL_Event* event)
{
    // raycast to select part
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            raycast_to_part();
            break;
        default: break;
    }
}

void mouse_button_up_handler(SDL_Event* event)
{}

void key_state_handler(Uint8 *keystate, int numkeys)
{}

void save()
{
    if (vox_dat == NULL) return;
    char fn[100] = {'\0'};
    char inter[70] = {'\0'};
    long int t = time(NULL);
    sprintf(inter, "./media/voxel/saves/%ld", t);
    sprintf(fn, "%s.skeleton", inter);
    vox_dat->save(fn);
}

}   // SkeletonEditor
