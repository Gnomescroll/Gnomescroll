#pragma once

#include <math.h>

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class DimensionComponent: public Component
{
    public:
        float height;
        float camera_height;

        float get_height() { return this->height; }
        int get_integer_height() { return (int)ceil(this->height); }
        float get_camera_height() { return this->camera_height; }
        Vec3 get_camera_position();

    DimensionComponent()
    : Component(COMPONENT_DIMENSION, COMPONENT_INTERFACE_DIMENSION),
    height(1.0f), camera_height(0.0f)
    {}
};

} // Objects
