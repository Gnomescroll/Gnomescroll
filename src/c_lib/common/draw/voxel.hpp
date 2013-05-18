#pragma once

#if DC_SERVER
# error Don't include this file in the server
#endif

namespace Draw
{

class Voxel
{
    private:

        void orient_vectors()
        {   // recalculates orientation vectors from angular parameter
            this->rotation_matrix = mat3_euler_rotation(this->theta, this->phi, 0.0f);
        }

    public:

        struct Mat3 rotation_matrix;

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

        void set_rotation_delta(float dtheta, float dphi)
        {
            this->dtheta_speed = dtheta;
            this->dphi_speed = dphi;
        }

        void delta_rotation(float dtheta_speed, float dphi_speed)
        {
            this->theta += dtheta_speed;
            this->phi += dphi_speed;
            this->orient_vectors();
        }

        void delta_rotation()
        {
            if (!this->dtheta_speed && !this->dphi_speed) return;
            this->set_rotation(this->theta + this->dtheta_speed, this->phi + this->dphi_speed);
        }

        void init()
        {
            this->orient_vectors();
        }

    Voxel() :
    size(1.0f), theta(0.0f), phi(0.0f), dtheta_speed(0.0f), dphi_speed(0.0f)
    {}
};

} // Draw
