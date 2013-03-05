#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class DestinationTargetingComponent: public TargetingComponent
{
    private:
        struct Vec3 destination;
    public:
        struct MapPos* path;
        size_t mpath;
        size_t ipath;
        float stop_proximity;
        float speed;
        int max_z_diff;
        float destination_choice_x;
        float destination_choice_y;
        bool at_destination;
        int ticks_to_destination;

    void check_target_alive();
    bool check_at_destination();
    void choose_destination();
    void orient_to_target(struct Vec3 camera_position);
    bool move_on_surface();
    void set_destination(struct Vec3 dest);

    struct Vec3 get_destination()
    {
        return this->destination;
    }

    bool path_finished()
    {
        return (this->path == NULL || this->ipath >= this->mpath);
    }

    int get_ticks_to_destination(Vec3 position)
    {
        if (vec3_equal(this->destination, position) ||
            this->speed == 0.0f)
        {
            return 0;
        }
        float len = vec3_length(vec3_sub(this->destination, position));
        int ttd = ceilf(len / this->speed);
        return ttd;
    }

    void adjust_speed(struct Vec3 pos)
    {
        if (this->ticks_to_destination <= 0 ||
            vec3_equal(this->destination, pos))
        {
            this->speed = 0.0f;
            return;
        }
        struct Vec3 direction = vec3_sub(this->destination, pos);
        float len = vec3_length(direction);
        this->speed = len / float(this->ticks_to_destination);
    }

    virtual ~DestinationTargetingComponent()
    {
        free(this->path);
    }

    DestinationTargetingComponent() :
        TargetingComponent(COMPONENT_DESTINATION_TARGETING),
        destination(vec3_init(0,0,0)),
        path(NULL), mpath(0), ipath(0),
        stop_proximity(0.1f),
        speed(1.0f), max_z_diff(128),
        destination_choice_x(1.0f), destination_choice_y(1.0f),
        at_destination(false),
        ticks_to_destination(1)
    {}
};

} // Components
