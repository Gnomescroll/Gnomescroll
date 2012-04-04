#pragma once

#include <c_lib/components/component.hpp>

// todo -- minivox color draw list

class MinivoxProperties: public ComponentProperties
{
    public:
        Color color;
        float size;
        Vec3 forward;
        Vec3 right;
        Vec3 normal;

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
            Vec3 forward, right, normal;
        
            forward.x = cos(theta * PI) * cos(phi * PI);
            forward.y = sin(theta * PI) * cos(phi * PI);
            forward.z = sin(phi);
            normalize_vector(&forward);

            right.x = cos(theta*PI + PI/2.0f);
            right.y = sin(theta*PI + PI/2.0f);
            right.z = 0.0f;
            normalize_vector(&right);

            normal = vec3_cross(forward, right);
            right = vec3_cross(forward, normal);

            forward = vec3_scalar_mult(forward, this->minivox_properties.size);
            right = vec3_scalar_mult(right, this->minivox_properties.size);
            normal = vec3_scalar_mult(normal, this->minivox_properties.size);

            this->minivox_properties.forward = forward;
            this->minivox_properties.right = right;
            this->minivox_properties.normal = normal;
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
    }
};
