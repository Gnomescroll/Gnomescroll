#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class MotionTargetingComponent: public TargetingComponent
{
    public:
        float speed;
        int max_z_down;
        int max_z_up;

        float destination_choice_x;
        float destination_choice_y;

        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;

        // lock to agent
        void lock_target(Vec3 camera_position);
        void lock_target(Vec3 camera_position, int team);

        void choose_destination();

        // lock to location
        //void lock_target_destination(Vec3 camera_position);

        void orient_to_target(Vec3 camera_position);

        bool move_on_surface();

        void broadcast_destination();

    MotionTargetingComponent()
    : TargetingComponent(COMPONENT_MOTION_TARGETING),
    speed(1.0f), max_z_down(128), max_z_up(128),
    destination_choice_x(1.0f), destination_choice_y(1.0f), 
    at_destination(false), en_route(false), ticks_to_destination(1)
    {}
};

} // Components
