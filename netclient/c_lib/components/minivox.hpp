#pragma once

#include <c_lib/components/component.hpp>

// todo -- minivox color draw list

class MinivoxProperties: public ComponentProperties
{
    public:
        Color color;
        float size;
        Vec3 forward;
        Vec3 normal;
        Vec3 right;

    MinivoxProperties()
    : size(1.0f)
    {}
};

class MinivoxComponent
{
    private:
        // TODO -- use euler matrix
        void orient_vectors(float theta, float phi)
        {   // recalculates orientation vectors from angular parameter
            this->minivox_properties.forward.x = cos(theta * PI) * cos(phi * PI);
            this->minivox_properties.forward.y = sin(theta * PI) * cos(phi * PI);
            this->minivox_properties.forward.z = sin(phi);
            normalize_vector(&this->minivox_properties.forward);

            this->minivox_properties.right.x = cos(theta*PI + PI/2.0f);
            this->minivox_properties.right.y = sin(theta*PI + PI/2.0f);
            this->minivox_properties.right.z = 0.0f;
            normalize_vector(&this->minivox_properties.right);

            this->minivox_properties.normal = vec3_cross(this->minivox_properties.forward, this->minivox_properties.right);
            this->minivox_properties.right = vec3_cross(this->minivox_properties.forward, this->minivox_properties.normal);

            this->minivox_properties.forward = vec3_scalar_mult(this->minivox_properties.forward, this->minivox_properties.size);
            this->minivox_properties.right = vec3_scalar_mult(this->minivox_properties.right, this->minivox_properties.size);
            this->minivox_properties.normal = vec3_scalar_mult(this->minivox_properties.normal, this->minivox_properties.size);
        }

    public:
        MinivoxProperties minivox_properties;

        Color get_color()
        {
            return this->minivox_properties.color;
        }

        void set_color(Color color)
        {
            this->minivox_properties.color = color;
        }

        void set_color(unsigned char r, unsigned char g, unsigned char b)
        {
            Color color;
            color.r = r;
            color.g = g;
            color.b = b;
            this->minivox_properties.color = color;
        }

        float get_minivox_size()
        {
            return this->minivox_properties.size;
        }

        void set_minivox_size(float size)
        {
            this->minivox_properties.size = size;
        }

        void set_rotation(float theta, float phi)
        {
            this->orient_vectors(theta, phi);
        }

    MinivoxComponent()
    {
        this->orient_vectors(0,0);
    }
};
