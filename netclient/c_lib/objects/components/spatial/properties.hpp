#pragma once

#include <c_lib/physics/verlet.hpp>

class PositionProperties
{
    public:
        Vec3 position;
        Vec3 angles;

    PositionProperties()
    {
        this->position = vec3_init(-1,-1,-1);
        this->angles = vec3_init(0,0,0);
    }
};

class PositionMomentumProperties: public PositionProperties
{
    public:
        Vec3 momentum;

    PositionMomentumProperties()
    : PositionProperties()
    {
        this->momentum = vec3_init(0,0,0);
    }
};

// DO NOT INSTANTIATE
// TODO -- group height,camera_height elsewhere
class StateChangedProperties
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

    StateChangedProperties()
    : _changed(false), camera_height(1.0f), height(1.0f)
    {}
};

class PositionChangedProperties: public PositionProperties, public StateChangedProperties
{
    public:
        PositionChangedProperties()
        : PositionProperties(), StateChangedProperties()
        {}
};


class PositionMomentumChangedProperties: public PositionMomentumProperties, public StateChangedProperties
{
    public:

    PositionMomentumChangedProperties()
    : PositionMomentumProperties(), StateChangedProperties()
    {}
};

class VerletProperties
{
    public:
        Verlet::VerletParticle vp;

    VerletProperties()
    {}
};


