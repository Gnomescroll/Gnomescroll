#pragma once

#if DC_CLIENT

#include <entity/constants.hpp>

namespace Components
{

class DrawComponent: public Component
{
    public:
        virtual void call() = 0;

    explicit DrawComponent(ComponentType type)
    : Component(type, COMPONENT_INTERFACE_DRAW)
    {}

    virtual ~DrawComponent() {}
};
    
} // Components

#endif
