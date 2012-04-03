#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/components/component.hpp>

/* List management */

// inherit this for a property list tracker
class BehaviourList
{
    private:
        virtual const char* name() = 0;
    public:
        ComponentProperties** objects;
        int max;
        int ct;

    void allocate()
    {
        if (this->max <= 0)
        {
            printf("WARNING: BehaviourList::allocate() -- attempt to allocate array of invalid size %d\n", this->max);
            return;
        }
        if (this->objects == NULL)
            this->objects = (ComponentProperties**)calloc(this->max, sizeof(ComponentProperties*));
        else
            printf("WARNING: BehaviourList::allocate() -- attempt to allocate more than once\n");
    }

    void register_object(ComponentProperties* state);
    void unregister_object(ComponentProperties* state);

    ~BehaviourList()
    {
        if (this->objects != NULL)
            free(this->objects);
    }
    explicit BehaviourList(int max)
    : objects(NULL), max(max), ct(0)
    {
        this->allocate();
    }
};
