#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <agent/agent.hpp>

// TODO: detect if player is on ground
// TODO: detect when player goes form on-ground to ground (fall event)
// TODO: detect the type of block the player is walking on
// TODO: random plus-minue variation in footstep

void player_agent_sound_ground_movement_event(class AgentState s0,  class AgentState s1, bool on_ground)
{
    static const float SOFT_FALL_VELOCITY_THRESHOLD = 1.0f;
    static const float HARD_FALL_VELOCITY_THRESHOLD = 2.0f;
    
    static const float distance_per_step = 1.5f;
    static float total_distance = 0.0f;

    static bool was_on_ground = on_ground;

    if (!was_on_ground && on_ground)
        printf("Landing with magnitude %0.2f\n", s0.vz);

    was_on_ground = on_ground;

    float dx = (s1.x - s0.x);
    float dy = (s1.y - s0.y);

    float d = sqrtf(dx*dx + dy*dy);

    total_distance += d;

    if (on_ground && s0.vz < 0.0f)
    {
        float dvz = s1.vz - s0.vz;
        if (dvz > HARD_FALL_VELOCITY_THRESHOLD)
            Sound::play_2d_sound("hard_fall.wav");
        else
        if (dvz > SOFT_FALL_VELOCITY_THRESHOLD)
            Sound::play_2d_sound("soft_fall.wav");
    }

    if (!on_ground) return;

    const int n_footsteps = 16;
    void (*footsteps[n_footsteps])(void) =
    {
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_3,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_4,
        
        Sound::soft_step_2,
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_2,
        Sound::soft_step_3,
        Sound::soft_step_1,
        Sound::soft_step_4,
        Sound::soft_step_2,
    };

    const int n_perturb_footsteps = 4;
    void (*perturb_footsteps[n_perturb_footsteps])(void) =
    {
        Sound::soft_step_1,
        Sound::soft_step_2,
        Sound::soft_step_3,
        Sound::soft_step_4,
    };

    
    #define RANDOM_STEPS 0
    #define RANDOM_CHANGE 1
    #define RANDOM_CHANGE_PER_LOOP 1

    if (total_distance > distance_per_step)
    {
        //play sound
        //printf("play agent sound !!! \n");
        //total_distance -= distance_per_step;
        total_distance = fmodf(total_distance, distance_per_step);
        //Sound::walking();
        #if RANDOM_STEPS
        footsteps[randrange(0,n_footsteps-1)]();
        #else
        static int step = 0;
        step++;
        step %= n_footsteps;
        if (RANDOM_CHANGE && randf() < (((float)RANDOM_CHANGE_PER_LOOP)/((float)n_footsteps)))
            perturb_footsteps[randrange(0,n_perturb_footsteps-1)]();
        else
            footsteps[step]();
        #endif
    }

    #undef RANDOM_STEPS
    #undef RANDOM_CHANGE
    #undef RANDOM_CHANGE_PER_LOOP
}
