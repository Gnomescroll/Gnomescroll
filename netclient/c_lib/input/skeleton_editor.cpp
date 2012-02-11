#include "skeleton_editor.hpp"

#include <input/handlers.hpp>

namespace SkeletonEditor
{
bool rotate = false;
Object_types type = OBJ_TYPE_AGENT;
int id = 0;
int part = 0;
VoxDat* vox_dat = NULL;
Voxel_model* vox = NULL;

float lateral_speed = 0.05f;
const int ROT_STEPS = 10;   // key presses for a full cycle
float rotation_speed = 2.0 / ROT_STEPS;

void move_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->sx += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void move_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->sy += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void move_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->sz += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void rotate_in_x(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->srx += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void rotate_in_y(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->sry += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void rotate_in_z(float s)
{
    if (vox_dat == NULL || vox == NULL) return;
    VoxPart* vp = vox_dat->vox_part[part];
    vp->srz += s;
    vp->set_local_matrix();    // TODO: use node
    vox->reset_skeleton(vox_dat);   // TODO maybe need to make this a reset switch
}

void raycast_to_part()
{
    // get camera vector
    float vec[3];
    current_camera->forward_vector(vec);

    // get camera position
    float x,y,z;
    x = current_camera->x;
    y = current_camera->y;
    z = current_camera->z;

    // hitscan against voxels
    float vox_distance = 10000000.0f;
    float collision_point[3];
    Voxel_hitscan_target target;
    bool voxel_hit = ClientState::voxel_hitscan_list.hitscan(
        x,y,z,
        vec[0], vec[1], vec[2],
        -1, (Object_types)-1,
        collision_point, &vox_distance,
        &target
    );

    if (!voxel_hit)
        return;

    id = target.entity_id;
    part = target.part_id;
    type = (Object_types)target.entity_type;

    void *obj;

    switch (type)
    {
        case OBJ_TYPE_AGENT:
            vox_dat = &agent_vox_dat;
            obj = ClientState::agent_list.get(id);
            if (obj==NULL) break;
            vox = ((Agent_state*)obj)->vox;
            break;
        case OBJ_TYPE_SLIME:
            vox_dat = &Monsters::slime_vox_dat;
            obj = ClientState::slime_list.get(id);
            if (obj==NULL) break;
            vox = ((Monsters::Slime*)obj)->vox;
            break;
        case OBJ_TYPE_SPAWNER:
            vox_dat = &spawner_vox_dat;
            obj = ClientState::spawner_list.get(id);
            if (obj==NULL) break;
            vox = ((Spawner*)obj)->vox;
            break;
        //case OBJ_TYPE_BASE:
            //vox_dat = &base_vox_dat;
            //obj = ClientState::ctf.get_base(id);
            //if (obj==NULL) break;
            //vox = ((Base*)obj)->vox;
            //break;
        //case OBJ_TYPE_FLAG:
            //vox_dat = &flag_vox_dat;
            //obj = ClientState::ctf.get_flag(id);
            //if (obj==NULL) break;
            //vox = ((Flag*)obj)->vox;
            //break;
        default: break;
    }
    //printf("Selected:\n");
    //printf("type=%d part=%d n_parts=%d\n", type, part, vox->n_parts);
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

        case SDLK_RETURN:
            save();
            break;
        
        case SDLK_LEFT:
            if (rotate)
                rotate_in_x(rotation_speed);
            else
                move_in_x(lateral_speed);
            break;
        case SDLK_RIGHT:
            if (rotate)
                rotate_in_x(-rotation_speed);
            else
                move_in_x(-lateral_speed);
            break;
        case SDLK_UP:
            if (rotate)
                rotate_in_y(rotation_speed);
            else
                move_in_y(lateral_speed);
            break;
        case SDLK_DOWN:
            if (rotate)
                rotate_in_y(-rotation_speed);
            else
                move_in_y(-lateral_speed);
            break;
        case SDLK_PERIOD:
            if (rotate)
                rotate_in_z(rotation_speed);
            else
                move_in_z(lateral_speed);
            break;
        case SDLK_COMMA:
            if (rotate)
                rotate_in_z(-rotation_speed);
            else
                move_in_z(-lateral_speed);
            break;

        case SDLK_RIGHTBRACKET: // toggle thru parts
            if (vox == NULL) break;
            part++;
            part %= vox->n_parts;
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
    sprintf(fn, "./media/voxel/saves/%d.skeleton", _GET_MS_TIME());
    vox_dat->save(fn);
}

}
