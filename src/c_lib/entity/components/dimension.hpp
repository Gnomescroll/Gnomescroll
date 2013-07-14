#pragma once

#include <math.h>

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class DimensionComponent: public Component
{
    private:
        bool camera_height_set;
        float height;
        float camera_height;
        float width;

    public:

    void set_height(float height)
    {
        this->height = height;
        if (!this->camera_height_set)
            this->camera_height = height;
    }

    void set_camera_height(float camera_height)
    {
        this->camera_height = camera_height;
        this->camera_height_set = true;
    }

    void set_width(float width)
    {
        this->width = width;
    }

    float get_height()
    {
        return this->height;
    }

    float get_width()
    {
        return this->width;
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
        COPY(width);
    }

    virtual ~DimensionComponent() {}

    DimensionComponent() :
        Component(COMPONENT_Dimension, COMPONENT_INTERFACE_Dimension),
        camera_height_set(false), height(1.0f), camera_height(1.0f),
        width(1.0f)
    {}
};

} // Entities
