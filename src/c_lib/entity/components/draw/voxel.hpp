#pragma once

#if DC_CLIENT

#include <entity/constants.hpp>
#include <entity/components/draw.hpp>

namespace Components
{

/*
 * Dont attach this component  (no type is defined)
 * Only hold common minivox stuff
 */

class VoxelComponent: public DrawComponent
{
    private:
        // TODO -- use euler matrix
        void orient_vectors()
        {   // recalculates orientation vectors from angular parameter
            Vec3 forward, right, normal;

            float theta = this->theta;
            float phi = this->phi;
            float size = this->size;
            
            forward.x = cosf(theta * PI) * cosf(phi * PI);
            forward.y = sinf(theta * PI) * cosf(phi * PI);
            forward.z = sinf(phi);
            normalize_vector(&forward);

            right.x = cosf(theta*PI + PI/2.0f);
            right.y = sinf(theta*PI + PI/2.0f);
            right.z = 0.0f;
            normalize_vector(&right);

            normal = vec3_cross(forward, right);
            normalize_vector(&normal);
            //right = vec3_cross(forward, normal);

            forward = vec3_scalar_mult(forward, size);
            right = vec3_scalar_mult(right, size);
            normal = vec3_scalar_mult(normal, size);

            this->forward = forward;
            this->right = right;
            this->normal = normal;
        }

    protected:
        Vec3 forward;
        Vec3 right;
        Vec3 normal;

    public:

        float size;
        float theta, phi;
        float dtheta_speed, dphi_speed;

        float get_minivox_size()
        {
            return this->size;
        }

        void set_minivox_size(float size)
        {
            this->size = size;
        }

        void set_rotation(float theta, float phi)
        {
            this->theta = theta;
            this->phi = phi;
            this->orient_vectors();
        }

        void delta_rotation(float dtheta_speed, float dphi_speed)
        {
            this->theta += dtheta_speed;
            this->phi += dphi_speed;
            this->orient_vectors();
        }

        void delta_rotation()
        {
            this->theta += this->dtheta_speed;
            this->phi += this->dphi_speed;
            this->orient_vectors();
        }

        Vec3 get_center(Vec3 position)
        {   // takes a positon vector and offsets it by half size in each dimension
            Vec3 center = position;
            float offset = this->size * 0.5f;
            center.x -= offset;
            center.y -= offset;
            return center;
        }

        void init()
        {
            this->orient_vectors();
        }

    explicit VoxelComponent(ComponentType type)
    : DrawComponent(type),
    size(1.0f), theta(0.0f), phi(0.0f), dtheta_speed(0.0f), dphi_speed(0.0f)
    {}
};

} // Components

#endif
