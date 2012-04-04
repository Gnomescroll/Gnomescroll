#pragma once

static bool position_is_equal(Vec3 p, float x, float y, float z)
{
    if (p.x != x || p.y != y || p.z != z)
        return false;
    return true;
}

class PositionProperties
{
    public:
        Vec3 position;
        float theta, phi;

    PositionProperties()
    : theta(0.0f), phi(0.0f)
    {
    }
};

class PositionVoxelProperties: public PositionProperties
{
    protected:
        bool _changed;
    public:
        float camera_height;
        float height;
        
        bool changed()
        {   // reset change state to false after a read
            bool cached = this->_changed;
            this->_changed = false;
            return cached;
        }
        void set_changed(bool changed)
        { this->_changed = changed; }

    PositionVoxelProperties()
    : _changed(false), camera_height(1.0f), height(1.0f)
    {}
};

class VerletProperties
{
    public:
        VerletParticle* vp;

    VerletProperties()
    : vp(NULL)
    {}

    ~VerletProperties()
    {
        if (this->vp != NULL)
            delete this->vp;
    }
};

class PositionComponent
{
    public:
    
        PositionProperties position_properties;

        Vec3 get_position()
        {
            return this->position_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->position_properties.position, x,y,z))
                return false;
            this->position_properties.position.x = x;
            this->position_properties.position.y = y;
            this->position_properties.position.z = z;
            return true;
        }

        Vec3 get_momentum()
        { return NULL_MOMENTUM; }
        void set_momentum(float x, float y, float z)
        {}
        float get_height()
        { return NULL_HEIGHT; }
        
    PositionComponent() {}
};

class PositionVoxelComponent
{
    public:
    
        PositionVoxelProperties position_properties;

        Vec3 get_position()
        {
            return this->position_properties.position;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->position_properties.position, x,y,z))
            {
                this->position_properties.set_changed(false);
                return false;
            }
            this->position_properties.position.x = x;
            this->position_properties.position.y = y;
            this->position_properties.position.z = z;

            this->position_properties.set_changed(true);
            return true;
        }

        float get_height()
        {
            return this->position_properties.height;
        }

        Vec3 get_momentum()
        { return NULL_MOMENTUM; }
        void set_momentum(float x, float y, float z)
        {}

        /* Additional specialization */

        float camera_z()
        {
            return this->position_properties.position.z + this->position_properties.camera_height;
        }

    PositionVoxelComponent() {}
};

class VerletComponent
{
    public:

        VerletProperties verlet_properties;

        /* Base api */
        
        Vec3 get_position()
        {
            return this->verlet_properties.vp->p;
        }

        bool set_position(float x, float y, float z)
        {
            if (position_is_equal(this->verlet_properties.vp->p, x,y,z))
                return false;
            this->verlet_properties.vp->set_position(x,y,z);
            return true;
        }

        Vec3 get_momentum()
        {
            return this->verlet_properties.vp->get_momentum();
        }

        void set_momentum(float x, float y, float z)
        {
            this->verlet_properties.vp->set_momentum(x,y,z);
        }

        /* Additional specialization */

        void create_particle(float x, float y, float z, float mx, float my, float mz)
        {
            const float mass = 1.0f; // TODO
            if (this->verlet_properties.vp == NULL)
                this->verlet_properties.vp = new VerletParticle(x,y,z, mx,my,mz, mass);
        }

        float get_height()
        { return NULL_HEIGHT; }

    VerletComponent()
    {
        this->create_particle(0,0,0,0,0,0);
    }

};
