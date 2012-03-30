#pragma once

template <class Super, class Object>
class TickVerletBounce: public Super
{
    protected:
    inline void tick(ObjectState* state, Object* object)
    {
        Verlet::bounce(state->vp, state->damp);
        Super::tick(state, object);
    }
};

template <class Super, class Object>
class TickTTL: public Super
{
    protected:
    inline void tick(ObjectState* state, Object* object)
    {
        if (state->ttl >= 0)
            state->ttl++;
        Super::tick(state, object);
    }
};

template <class Super, class Object>
class TickStayOnGround: public Super
{
    protected:
    inline void tick(ObjectState* state, Object* object)
    {
        #if DC_SERVER
        // fall/climb with terrain
        Vec3 p = state->get_position();
        int x = (int)p.x;
        int y = (int)p.y;
        int z = (int)p.z;
        if (isSolid(_get(x,y,z)))
        {   // move up
            while (isSolid(_get(x,y,++z)))
                if (z >= map_dim.z)
                {
                    z = map_dim.z;
                    break;
                }
        }
        else
        {   // fall down
            while (!isSolid(_get(x,y,--z)))
                if (z<=0)
                {
                    z = 0;
                    break;
                }
            z++;
        }
        state->set_position(p.x, p.y, (float)z);
        #endif
        Super::tick(state, object);
    }
};

template <class Super, class Object>
class TickTargetAcquisition: public Super
{
    protected:
    inline void tick(ObjectState* state, Object* object)
    {
        #if DC_SERVER
        if (state->fire_tick % state->fire_rate_limit == 0)
            object->acquire_target(state);
        state->fire_tick++;
        #endif
        Super::tick(state, object);
    }
};
