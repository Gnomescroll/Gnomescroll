#pragma once

#include <particle/particles.hpp>
#include <hud/text.hpp>

namespace Particle
{

const float DEFAULT_MASS = 1.0f;

class TextParticle: public ParticleMotionCore, public HudText::Text
{
    public:
        int id;

        TextParticle() :
            ParticleMotionCore(0,0,0,0,0,0, DEFAULT_MASS),
            HudText::Text(),
            id(-1)
            {}
};

}   // Particles
