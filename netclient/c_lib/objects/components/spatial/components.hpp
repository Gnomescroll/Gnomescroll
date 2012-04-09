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
        void set_momentum(float x, float y, float z)
        {}
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

        void set_momentum(float x, float y, float z)
        {
            this->spatial_properties.momentum.x = x;
            this->spatial_properties.momentum.y = y;
            this->spatial_properties.momentum.z = z;
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
        void set_momentum(float x, float y, float z)
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

        void set_momentum(float x, float y, float z)
        {
            this->spatial_properties.momentum.x = x;
            this->spatial_properties.momentum.y = y;
            this->spatial_properties.momentum.z = z;
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
            return this->verlet_properties.vp.p;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->verlet_properties.vp.p, x,y,z))
                return false;
            this->verlet_properties.vp.set_position(x,y,z);
            return true;
        }

        Vec3 get_momentum()
        {
            return this->verlet_properties.vp.get_momentum();
        }

        void set_momentum(float x, float y, float z)
        {
            this->verlet_properties.vp.set_momentum(x,y,z);
        }

        /* Additional specialization */

        void create_particle(float x, float y, float z, float mx, float my, float mz)
        {
            const float mass = 1.0f; // TODO
            this->verlet_properties.vp.mass = mass;
            this->verlet_properties.vp.set_state(x,y,z,mx,my,mz);
        }

        float get_height()
        { return NULL_HEIGHT; }

        Vec3 get_angles() { return NULL_ANGLES; }
        void set_angles(float theta, float phi, float rho)
        {}

    VerletComponent()
    {
        this->create_particle(0,0,0,0,0,0);
    }
};
