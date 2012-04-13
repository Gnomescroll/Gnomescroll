#pragma once

#if DC_CLIENT

#include <c_lib/objects/common/component/component.hpp>

// todo -- minivox color draw list

class MinivoxProperties: public ComponentProperties
{
    public:
        Color color;
        float size;
        float theta, phi;
        float dtheta_speed, dphi_speed;
        Vec3 forward;
        Vec3 right;
        Vec3 normal;

    MinivoxProperties()
    :
    size(1.0f),
    theta(0.0f), phi(0.0f),
    dtheta_speed(0.0f), dphi_speed(0.0f)
    {}
};

class MinivoxComponent
{
    private:
        // TODO -- use euler matrix
        void orient_vectors()
        {   // recalculates orientation vectors from angular parameter
            Vec3 forward, right, normal;

            float theta = this->minivox_properties.theta;
            float phi = this->minivox_properties.phi;
            float size = this->minivox_properties.size;
            
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

            forward = vec3_scalar_mult(forward, size);
            right = vec3_scalar_mult(right, size);
            normal = vec3_scalar_mult(normal, size);

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
            this->minivox_properties.theta = theta;
            this->minivox_properties.phi = phi;
            this->orient_vectors();
        }

        void delta_rotation(float dtheta, float dphi)
        {
            this->minivox_properties.theta += dtheta;
            this->minivox_properties.phi += dphi;
            this->orient_vectors();
        }

        void delta_rotation()
        {
            this->minivox_properties.theta += this->minivox_properties.dtheta_speed;
            this->minivox_properties.phi += this->minivox_properties.dphi_speed;
            this->orient_vectors();
        }

        Vec3 get_center(Vec3 position)
        {   // takes a positon vector and offsets it by half size in each dimension
            Vec3 center;
            float offset = this->minivox_properties.size * 0.5f;
            center.x = position.x - offset;
            center.y = position.y - offset;
            center.z = position.z;
            return center;
        }

    MinivoxComponent()
    {
    }
};

#endif
