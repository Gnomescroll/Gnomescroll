#pragma once

#include <math.h>

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class DimensionComponent: public Component
{
    public:
        float height;
        float camera_height;

    float get_height()
    {
        return this->height;
    }

    int get_integer_height()
    {
        int h = int(ceilf(this->height));
        IF_ASSERT(h < 0) h = 1;
        return h;
    }

    float get_camera_height()
    {
        return this->camera_height;
    }

    Vec3 get_camera_position();

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(DimensionComponent);
        COPY(height);
        COPY(camera_height);
    }

    DimensionComponent() :
        Component(COMPONENT_Dimension, COMPONENT_INTERFACE_Dimension),
        height(1.0f), camera_height(0.0f)
    {}
};

} // Entities
