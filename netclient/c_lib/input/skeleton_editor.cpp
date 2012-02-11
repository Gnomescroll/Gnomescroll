#include "skeleton_editor.hpp"

#include <input/handlers.hpp>

namespace SkeletonEditor
{

Object_types type = OBJ_TYPE_AGENT;
int n_parts = 1;
int part = 0;

float lateral_speed = 0.05f;
float rotation_speed = 0.05f;

void move_in_x(float s)
{
printf("motion x %0.2f\n", s);
}

void move_in_y(float s)
{
printf("motion y %0.2f\n", s);
}

void move_in_z(float s)
{
printf("motion z %0.2f\n", s);
}

void rotate_in_x(float s)
{
printf("rotate x %0.2f\n", s);
}

void rotate_in_y(float s)
{
printf("rotate y %0.2f\n", s);
}

void rotate_in_z(float s)
{
printf("rotate z %0.2f\n", s);
}

void select_model(int type, int part)
{
    // need vox_dat attached to vox model probably
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

    part = target.part_id;
    type = (Object_types)target.entity_type;
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
        
        case SDLK_LEFT:
            move_in_x(lateral_speed);
            break;
        case SDLK_RIGHT:
            move_in_x(-lateral_speed);
            break;
        case SDLK_UP:
            move_in_y(lateral_speed);
            break;
        case SDLK_DOWN:
            move_in_y(-lateral_speed);
            break;
        case SDLK_PERIOD:
            move_in_z(lateral_speed);
            break;
        case SDLK_COMMA:
            move_in_z(-lateral_speed);
            break;

        case SDLK_RIGHTBRACKET: // toggle thru parts
            part++;
            part %= n_parts;
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

void init()
{}

void update()
{}

void save()
{
}

}
