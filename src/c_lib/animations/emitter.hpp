#pragma once

#include <animations/mining_laser.hpp>
#include <camera/fulstrum_test.hpp>

namespace Animations
{

/*
 *  Give one of these to each agent
 * 

        Turn on: draw all particles
        Turn off: stop draw all particles

        all particles have:
            jitter (+/- small amt)


        tick each particle
        move last one to beginning

        raycast to nearest block
        calculate % distance of full length
        multiply by ct
        stop drawing before this ct

 */

const int MINING_LASER_EMITTER_PARTICLE_COUNT = 100;
const float MINING_LASER_PARTICLE_SPEED = 6.0f;

class MiningLaserEmitter
{
    public:
    
        bool on;
        struct Vec3 position;
        struct Vec3 direction;
        struct Vec3 velocity;
        int count;
        class MiningLaser particles[MINING_LASER_EMITTER_PARTICLE_COUNT];
        float speed;
        float base_length;
        float length;
        int ttl_max;
        int start;
        float h_mult;   // height adjustment for sprite

    bool in_frustum()
    {
        // center point in middle of beam, use sphere test
        struct Vec3 center = vec3_add(this->position, vec3_scalar_mult(this->direction, this->length * 0.5f));
        return sphere_fulstrum_test(center.x, center.y, center.z, this->length * 0.5f);
    }
    
    void prep_draw()
    {
        if (!this->on) return;
        if (!this->in_frustum()) return;

        int ct = this->get_render_count();
        for (int i=0; i<ct; i++)
            this->particles[(i+this->start)%this->count].prep(h_mult);
    }

    void tick()
    {
        if (!this->on) return;

        this->set_count();
        this->update_length();

        this->start = -1;
        for (int i=0; i<this->count; i++)
        {
            this->particles[i].tick();
            if (this->particles[i].ttl >= this->ttl_max)
            {
                this->particles[i].ttl = 0;
                this->particles[i].update_type();
                float jitter = randf() * 0.1f;
                this->particles[i].verlet.position = vec3_add(this->position, vec3_scalar_mult(this->direction, jitter));
            }
            if (this->start < 0 && this->particles[i].ttl == 0) this->start = i;
        }
        if (this->start < 0) this->start = 0;
    }

    void set_state(struct Vec3 p, struct Vec3 f)
    {
        if (vec3_equal(this->position, p) && vec3_equal(this->direction, f)) return;
        this->position = p;
        this->direction = f;
        this->velocity = vec3_scalar_mult(f, this->speed);

        float ddv = (1.0f/(float)this->ttl_max) * this->length;
        ddv /= ((float)this->count) / ((float)this->ttl_max);
        int last = -1;
        int j = 0;
        for (int i=0; i<this->count; i++)
        {
            float dv = ((float)this->particles[i].ttl)/((float)this->ttl_max);
            float len = this->base_length * dv;
            this->particles[i].verlet.position = vec3_add(this->position, vec3_scalar_mult(this->direction, len));
            this->particles[i].verlet.position = vec3_add(this->particles[i].verlet.position, vec3_scalar_mult(this->direction, ddv*j));
            this->particles[i].verlet.velocity = this->velocity;
            if (last == this->particles[i].ttl) j++;
            else j = 0;
            last = this->particles[i].ttl;
        }
    }

    void realign_particles()
    {
        for (int i=0; i<this->count; i++)
        {
            float pct = ((float)i)/((float)this->count);
            this->particles[i].ttl = this->ttl_max * pct;
        }
    }

    void set_count()
    {
        float scale = ((float)Options::animation_level)/3.0f;
        int ct = scale*((float)MINING_LASER_EMITTER_PARTICLE_COUNT);
        if (ct == this->count) return;
        this->count = ct;
        this->realign_particles();
    }

    void set_base_length(float base_length)
    {
        this->ttl_max = (this->base_length/this->speed)*30;
        if (this->base_length == base_length) return;
        this->base_length = base_length;
        this->realign_particles();
    }

    void update_length()
    {
        float interval = 1.0f;
        struct Vec3 dest = vec3_add(this->position, vec3_scalar_mult(this->direction, this->base_length));
        ray_cast_interval(this->position, dest, &interval);
        this->length = interval*this->base_length;
    }

    int get_render_count()
    {
        int ct = (int)((this->length/this->base_length) * ((float)this->count));
        return ct;
    }

    void turn_on()
    {
        if (this->on) return;
        this->on = true;
        this->set_count();
    }

    void turn_off()
    {
        this->on = false;
    }

    MiningLaserEmitter()
    : on(false), position(vec3_init(0,0,0)), direction(vec3_init(1,0,0)),
    count(MINING_LASER_EMITTER_PARTICLE_COUNT), speed(MINING_LASER_PARTICLE_SPEED),
    base_length(4.0f), length(4.0f), ttl_max((base_length/speed)*30), start(0), h_mult(0.0f)
    {
        this->set_count();
        this->set_base_length(4.0f);
        this->realign_particles();
    }
};

}   // Animations
