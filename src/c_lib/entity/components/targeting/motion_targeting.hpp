#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class MotionTargetingComponent: public TargetingComponent
{
    protected:
        int ticks_locked;
    public:
        // config
        float speed;
        int max_z_diff;
        int max_lock_ticks;
        float destination_choice_x;
        float destination_choice_y;

        // state
        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;

    void check_target_alive();
    // explicitly set the target
    void set_target(EntityType target_type, int target_id);
    void lock_target(Vec3 camera_position);
    void choose_destination();
    void orient_to_target(Vec3 camera_position);
    void move_on_surface();
    void call();

    virtual ~MotionTargetingComponent() {}

    MotionTargetingComponent() :
        TargetingComponent(COMPONENT_MOTION_TARGETING),
        ticks_locked(0),
        speed(1.0f), max_z_diff(128), max_lock_ticks(0),
        destination_choice_x(1.0f), destination_choice_y(1.0f),
        destination(vec3_init(0)), at_destination(false), en_route(false),
        ticks_to_destination(1)
    {}
};

} // Components
