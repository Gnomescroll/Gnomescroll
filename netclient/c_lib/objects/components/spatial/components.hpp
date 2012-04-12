#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/objects/components/spatial/properties.hpp>

static bool position_is_equal(Vec3 p, float x, float y, float z)
{
    if (p.x != x || p.y != y || p.z != z)
        return false;
    return true;
}

class PositionComponent: public SpatialDelegate
{
    public:
    
        PositionProperties properties;

        Vec3 get_position()
        {
            return this->properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->properties.position, x,y,z))
                return false;
            this->properties.position.x = x;
            this->properties.position.y = y;
            this->properties.position.z = z;
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
            return this->properties.angles;
        }
        bool set_angles(float theta, float phi, float rho)
        {
            if (position_is_equal(this->properties.angles, theta, phi, rho))
                return false;
            this->properties.angles.x = theta;
            this->properties.angles.y = phi;
            this->properties.angles.z = rho;
            return true;
        }

        
    PositionComponent()
    {
    }
};

class PositionMomentumComponent: public SpatialDelegate
{
    public:
    
        PositionMomentumProperties properties;

        Vec3 get_position()
        {
            return this->properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->properties.position, x,y,z))
                return false;
            this->properties.position.x = x;
            this->properties.position.y = y;
            this->properties.position.z = z;
            return true;
        }

        Vec3 get_momentum()
        {
            return this->properties.momentum;
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->properties.momentum.x = mx;
            this->properties.momentum.y = my;
            this->properties.momentum.z = mz;
        }
        
        float get_height()
        { return NULL_HEIGHT; }
        
        Vec3 get_angles()
        {
            return this->properties.angles;
        }
        bool set_angles(float theta, float phi, float rho)
        {
            if (position_is_equal(this->properties.angles, theta, phi, rho))
                return false;
            this->properties.angles.x = theta;
            this->properties.angles.y = phi;
            this->properties.angles.z = rho;
            return true;
        }


    PositionMomentumComponent()
    {
    }
};

class PositionChangedComponent: public SpatialDelegate
{
    public:
    
        PositionChangedProperties properties;

        Vec3 get_position()
        {
            return this->properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->properties.position, x,y,z))
                return false;
            this->properties.position.x = x;
            this->properties.position.y = y;
            this->properties.position.z = z;

            this->properties.set_changed(true);
            return true;
        }

        float get_height()
        {
            return this->properties.height;
        }

        Vec3 get_angles()
        {
            return this->properties.angles;
        }
        bool set_angles(float theta, float phi, float rho)
        {
            if (position_is_equal(this->properties.angles, theta, phi, rho))
                return false;
            this->properties.angles.x = theta;
            this->properties.angles.y = phi;
            this->properties.angles.z = rho;

            this->properties.set_changed(true);
            return true;
        }

        Vec3 get_momentum()
        { return NULL_MOMENTUM; }
        void set_momentum(float mx, float my, float mz)
        {}

        /* Additional specialization */

        float camera_z()
        {
            return this->properties.position.z + this->properties.camera_height;
        }

    PositionChangedComponent() {}
};

class PositionMomentumChangedComponent: public SpatialDelegate
{
    public:
    
        PositionMomentumChangedProperties properties;

        Vec3 get_position()
        {
            return this->properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->properties.position, x,y,z))
                return false;
            this->properties.position.x = x;
            this->properties.position.y = y;
            this->properties.position.z = z;

            this->properties.set_changed(true);
            return true;
        }

        float get_height()
        {
            return this->properties.height;
        }

        Vec3 get_momentum()
        {
            return this->properties.momentum;
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->properties.momentum.x = mx;
            this->properties.momentum.y = my;
            this->properties.momentum.z = mz;
        }

        Vec3 get_angles()
        {
            return this->properties.angles;
        }
        bool set_angles(float theta, float phi, float rho)
        {
            if (position_is_equal(this->properties.angles, theta, phi, rho))
                return false;
            this->properties.angles.x = theta;
            this->properties.angles.y = phi;
            this->properties.angles.z = rho;

            this->properties.set_changed(true);
            return true;
        }

        /* Additional specialization */

        float camera_z()
        {
            return this->properties.position.z + this->properties.camera_height;
        }

    PositionMomentumChangedComponent() {}
};

class VerletComponent: public SpatialDelegate
{
    public:

        VerletProperties properties;

        /* Base api */
        
        Vec3 get_position()
        {
            return this->properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->properties.position, x,y,z))
                return false;
            this->properties.old_position = this->properties.position;
            this->properties.position = vec3_init(x,y,z);
            return true;
        }

        Vec3 get_momentum()
        {
            return vec3_scalar_mult(this->properties.velocity, this->properties.mass);
        }

        void set_momentum(float mx, float my, float mz)
        {
            this->properties.old_velocity = this->properties.velocity;
            this->properties.velocity = vec3_scalar_mult(vec3_init(mx,my,mz), 1.0f/this->properties.mass);
        }
        
        void set_momentum(Vec3 momentum)
        {
            this->properties.old_velocity = this->properties.velocity;
            this->properties.velocity = vec3_scalar_mult(momentum, 1.0f/this->properties.mass);
        }

        float get_height()
        { return NULL_HEIGHT; }
        Vec3 get_angles() { return NULL_ANGLES; }
        bool set_angles(float theta, float phi, float rho)
        { return false; }

        /* Addition specialization */

        void set_mass(float mass)
        {
            Vec3 momentum = this->get_momentum();
            this->properties.mass = mass;
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
                &this->properties.old_position,
                &this->properties.old_velocity,
                &this->properties.position,
                &this->properties.velocity,
                damp
            );
        }

    VerletComponent()
    {
    }
};
