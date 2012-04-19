#pragma once

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
        float get_camera_height() { return this->camera_height; }
        Vec3 get_camera_position();

    OwnerComponent()
    : Component(COMPONENT_DIMENSION, COMPONENT_INTERFACE_DIMENSION),
    owner(NO_OWNER)
    {}
};

} // Objects
