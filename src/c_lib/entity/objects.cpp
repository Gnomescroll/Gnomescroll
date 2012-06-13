#include "objects.hpp"

#include <entity/object/main.hpp>

namespace Objects
{

Object* get(ObjectType type, int id)
{
    return get_object(type,id);
}

void ready(Object* object)
{
    ready_switch(object);
}

void destroy(Object* object)
{
    destroy_switch(object);
}

void destroy(ObjectType type, int id)
{
    destroy_switch(type, id);
}

void stress_test()
{
    ObjectType type = OBJECT_MONSTER_BOMB;
    const int iters = 1000;
    printf("Begin object stress test, maxing out %d %d times\n", type, iters);
    int ttl = 0;
    for (int i=0; i<iters; i++)
    {
        //printf("%d\n", i);
        Object* obj = NULL;
        int n = 0;
        while((obj = create(type)) != NULL)
        {
            n++;
            ready(obj);
        }
        //printf("Maxed out %d to %d\n", type, n);
        //printf("Destroy %d of %d\n", n, type);
        for (int i=0; i<n; i++)
            destroy(type, i);
        //printf("Destroyed %d\n", n);
        ttl += n;
    }
    printf("Stress test done. Created and destroyed %d objects of type %d\n", iters*ttl, type);
}

} // Objects
