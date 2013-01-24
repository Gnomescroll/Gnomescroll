#include "equipped_sprite_adjuster.hpp"

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#include <animations/weapon.hpp>

/*
 * Controls
 * 
 * q - toggle adjustment mode between
 *     the focal distance point, or the sprite origin 
 * 
 * Left/Right arrow - move point in xy plane
 * Up/down arrow - move point in z
 * ,. - move point's depth from camera plane
 * 
 * r - reset point
 * p - print point
 * 
 * +/- - change delta step size 
 * [] - adjust sprite scale
 */

namespace EquippedSpriteAdjuster
{

using Animations::move_focal_depth;
using Animations::move_focal_vertical;
using Animations::move_focal_horizontal;
using Animations::move_origin_depth;
using Animations::move_origin_vertical;
using Animations::move_origin_horizontal;
using Animations::dilate_equipped_sprite;
using Animations::print_sprite_alignment_config;

static float delta = 0.05f;
static float sprite_size_delta = 0.01f;

static bool move_focal = true;
static bool move_origin = false;

static char* point_name = (char*)"focal point";

static float focal_depth_total = 0.0f;
static float focal_horizontal_total = 0.0f;
static float focal_vertical_total = 0.0f;
static float origin_depth_total = 0.0f;
static float origin_horizontal_total = 0.0f;
static float origin_vertical_total = 0.0f;

void key_down_handler(SDL_Event* event)
{
    GS_ASSERT(event != NULL);
    if (event == NULL) return;
    
    switch (event->key.keysym.sym)
    {
        // toggle focal/origin adjustment
        case SDLK_q:
            move_focal = !move_focal;
            move_origin = !move_origin;
            if (move_focal && move_origin) point_name = (char*)"both points";
            if (move_focal) point_name = (char*)"focal point";
            if (move_origin) point_name = (char*)"origin point";
            printf("Using %s\n", point_name);
            break;
            
        // reset point, by undoing cumulative deltas applied by the editor
        case SDLK_r:
            move_focal_depth(focal_depth_total);
            move_focal_horizontal(focal_horizontal_total);
            move_focal_vertical(focal_vertical_total);
            move_origin_depth(origin_depth_total);
            move_origin_horizontal(origin_horizontal_total);
            move_origin_vertical(origin_vertical_total);
            printf("Reset %s\n", point_name);
            break;
            
        case SDLK_p:
            print_sprite_alignment_config();
            break;
            
        // position adjustments
        case SDLK_UP:
            if (move_focal)
            {
                move_focal_vertical(delta);
                focal_vertical_total += delta;
            }
            if (move_origin)
            {
                move_origin_vertical(delta);
                origin_vertical_total += delta;
            }
            break;
            
        case SDLK_DOWN:
            if (move_focal)
            {
                move_focal_vertical(-delta);
                focal_vertical_total += -delta;
            }
            if (move_origin)
            {
                move_origin_vertical(-delta);
                focal_vertical_total += -delta;
            }
            break;
                        
        case SDLK_RIGHT:
            if (move_focal)
            {
                move_focal_horizontal(delta);
                focal_horizontal_total += delta;
            }
            if (move_origin)
            {
                move_origin_horizontal(delta);
                origin_horizontal_total += delta;
            }
            break;
        
        case SDLK_LEFT:
            if (move_focal)
            {
                move_focal_horizontal(-delta);
                focal_horizontal_total += -delta;
            }
            if (move_origin)
            {
                move_origin_horizontal(-delta);
                origin_horizontal_total += -delta;
            }
            break;

        case SDLK_PERIOD:
            if (move_focal)
            {
                move_focal_depth(delta);
                focal_depth_total += delta;
            }
            if (move_origin)
            {
                move_origin_depth(delta);
                origin_depth_total += delta;
            }
            break;

        case SDLK_COMMA:
            if (move_focal)
            {
                move_focal_depth(-delta);
                focal_depth_total += -delta;
            }
            if (move_origin)
            {
                move_origin_depth(-delta);
                origin_depth_total += -delta;
            }
            break;
            
        // delta adjustment
        case SDLK_EQUALS: // +
            delta += 0.1f;
            printf("delta: %f\n", delta);
            break;
        case SDLK_MINUS:  // -
            if (delta > 0.1f)   // dont decrement below 0.1f
                delta -= 0.1f;
            printf("delta: %f\n", delta);
            break;
            
        // item sprite
        case SDLK_LEFTBRACKET:
            dilate_equipped_sprite(-sprite_size_delta);
            break;

        case SDLK_RIGHTBRACKET:
            dilate_equipped_sprite(sprite_size_delta);
            break;
                        
        default:
            break;
    }
}

void key_up_handler(SDL_Event* event)
{

}

void mouse_button_down_handler(SDL_Event* event)
{

}

void mouse_button_up_handler(SDL_Event* event)
{

}

void key_state_handler(Uint8 *keystate, int numkeys)
{

}

}   // EquippedSpriteAdjuster

#ifdef __clang__
# pragma clang diagnostic pop
#endif
