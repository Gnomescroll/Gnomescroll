#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/components/component.hpp>

/* List management */

// inherit this for a property list tracker
class BehaviourList
{
    private:
        virtual const char* name() = 0;

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

    public:
        ComponentProperties** objects;
        int max;
        int ct;

        bool full()
        {
            if (this->ct >= this->max)
                return true;
            return false;
        }

        void register_object(ComponentProperties* state);
        void unregister_object(ComponentProperties* state);

    virtual ~BehaviourList()
    {
        if (this->objects != NULL)
            free(this->objects);
    }

    BehaviourList()
    : objects(NULL), max(0), ct(0)
    {}
    
    explicit BehaviourList(int max)
    : objects(NULL), max(max), ct(0)
    {
        this->allocate();
    }
};
