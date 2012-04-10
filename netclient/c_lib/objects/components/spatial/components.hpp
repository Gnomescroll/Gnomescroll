#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/objects/components/spatial/properties.hpp>

static bool position_is_equal(Vec3 p, float x, float y, float z)
{
    if (p.x != x || p.y != y || p.z != z)
        return false;
    return true;
}

class PositionComponent
{
    public:
    
        PositionProperties spatial_properties;

        Vec3 get_position()
        {
            return this->spatial_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->spatial_properties.position, x,y,z))
                return false;
            this->spatial_properties.position.x = x;
            this->spatial_properties.position.y = y;
            this->spatial_properties.position.z = z;
            return true;
        }

        Vec3 get_momentum()
        { return NULL_MOMENTUM; }
        void set_momentum(float mx, float my, float mz)
        {}
        float get_height()
        { return NULL_HEIGHT; }

        Vec3 get_angles()
        {
            return this->spatial_properties.angles;
        }
        void set_angles(float theta, float phi, float rho)
        {   // TODO -- euler vector
            this->spatial_properties.angles.x = theta;
            this->spatial_properties.angles.y = phi;
            this->spatial_properties.angles.z = rho;
        }
        
    PositionComponent()
    {
    }
};

class PositionMomentumComponent
{
    public:
    
        PositionMomentumProperties spatial_properties;

        Vec3 get_position()
        {
            return this->spatial_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->spatial_properties.position, x,y,z))
                return false;
            this->spatial_properties.position.x = x;
            this->spatial_properties.position.y = y;
            this->spatial_properties.position.z = z;
            return true;
        }

        Vec3 get_momentum()
        {
            return this->spatial_properties.momentum;
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->spatial_properties.momentum.x = mx;
            this->spatial_properties.momentum.y = my;
            this->spatial_properties.momentum.z = mz;
        }
        
        float get_height()
        { return NULL_HEIGHT; }
        
        Vec3 get_angles()
        {
            return this->spatial_properties.angles;
        }
        void set_angles(float theta, float phi, float rho)
        {
            this->spatial_properties.angles.x = theta;
            this->spatial_properties.angles.y = phi;
            this->spatial_properties.angles.z = rho;
        }

    PositionMomentumComponent()
    {
    }
};

class PositionChangedComponent
{
    public:
    
        PositionChangedProperties spatial_properties;

        Vec3 get_position()
        {
            return this->spatial_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->spatial_properties.position, x,y,z))
            {
                this->spatial_properties.set_changed(false);
                return false;
            }
            this->spatial_properties.position.x = x;
            this->spatial_properties.position.y = y;
            this->spatial_properties.position.z = z;

            this->spatial_properties.set_changed(true);
            return true;
        }

        float get_height()
        {
            return this->spatial_properties.height;
        }

        Vec3 get_angles()
        {
            return this->spatial_properties.angles;
        }
        void set_angles(float theta, float phi, float rho)
        {
            this->spatial_properties.angles.x = theta;
            this->spatial_properties.angles.y = phi;
            this->spatial_properties.angles.z = rho;
        }

        Vec3 get_momentum()
        { return NULL_MOMENTUM; }
        void set_momentum(float mx, float my, float mz)
        {}

        /* Additional specialization */

        float camera_z()
        {
            return this->spatial_properties.position.z + this->spatial_properties.camera_height;
        }

    PositionChangedComponent() {}
};

class PositionMomentumChangedComponent
{
    public:
    
        PositionMomentumChangedProperties spatial_properties;

        Vec3 get_position()
        {
            return this->spatial_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->spatial_properties.position, x,y,z))
            {
                this->spatial_properties.set_changed(false);
                return false;
            }
            this->spatial_properties.position.x = x;
            this->spatial_properties.position.y = y;
            this->spatial_properties.position.z = z;

            this->spatial_properties.set_changed(true);
            return true;
        }

        float get_height()
        {
            return this->spatial_properties.height;
        }

        Vec3 get_momentum()
        {
            return this->spatial_properties.momentum;
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->spatial_properties.momentum.x = mx;
            this->spatial_properties.momentum.y = my;
            this->spatial_properties.momentum.z = mz;
        }

        Vec3 get_angles()
        {
            return this->spatial_properties.angles;
        }
        void set_angles(float theta, float phi, float rho)
        {
            this->spatial_properties.angles.x = theta;
            this->spatial_properties.angles.y = phi;
            this->spatial_properties.angles.z = rho;
        }

        /* Additional specialization */

        float camera_z()
        {
            return this->spatial_properties.position.z + this->spatial_properties.camera_height;
        }

    PositionMomentumChangedComponent() {}
};

class VerletComponent
{
    public:

        VerletProperties verlet_properties;

        /* Base api */
        
        Vec3 get_position()
        {
            return this->verlet_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->verlet_properties.position, x,y,z))
                return false;
            this->verlet_properties.old_position = this->verlet_properties.position;
            this->verlet_properties.position = vec3_init(x,y,z);
            return true;
        }

        Vec3 get_momentum()
        {
            return vec3_scalar_mult(this->verlet_properties.velocity, this->verlet_properties.mass);
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->verlet_properties.old_velocity = this->verlet_properties.velocity;
            this->verlet_properties.velocity = vec3_scalar_mult(vec3_init(mx,my,mz), 1.0f/this->verlet_properties.mass);
        }
        
        void set_momentum(Vec3 momentum)
        {
            this->verlet_properties.old_velocity = this->verlet_properties.velocity;
            this->verlet_properties.velocity = vec3_scalar_mult(momentum, 1.0f/this->verlet_properties.mass);
        }

        float get_height()
        { return NULL_HEIGHT; }
        Vec3 get_angles() { return NULL_ANGLES; }
        void set_angles(float theta, float phi, float rho)
        {}

        /* Addition specialization */

        void set_mass(float mass)
        {
            Vec3 momentum = this->get_momentum();
            this->verlet_properties.mass = mass;
            this->set_momentum(momentum);
        }

        void set_state(float x, float y, float z, float mx, float my, float mz)
        {
            this->set_position(x,y,z);
            this->set_momentum(mx,my,mz);
        }

        bool verlet_bounce(float damp)
        {
            return Verlet::bounce(
                &this->verlet_properties.old_position,
                &this->verlet_properties.old_velocity,
                &this->verlet_properties.position,
                &this->verlet_properties.velocity,
                damp
            );
        }

    VerletComponent()
    {
    }
};
