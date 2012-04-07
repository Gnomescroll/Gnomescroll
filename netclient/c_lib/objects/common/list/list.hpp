#pragma once

#include <c_lib/objects/common/interface/policy.hpp>

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

// inherit this for a property list tracker
// use the template version for nonstandard properties
template <class Property>
class PropertyList
{
    private:
        virtual const char* name() = 0;

        void allocate()
        {
            if (this->max <= 0)
            {
                printf("WARNING: PropertyList::allocate() -- attempt to allocate array of invalid size %d\n", this->max);
                return;
            }
            if (this->objects == NULL)
                this->objects = (Property**)calloc(this->max, sizeof(Property*));
            else
                printf("WARNING: PropertyList::allocate() -- attempt to allocate more than once\n");
        }

    public:
        Property** objects;
        int max;
        int ct;

        void print() { printf("%s property list\n", this->name()); }

        bool full()
        {
            if (this->ct >= this->max)
                return true;
            return false;
        }

        void register_object(Property* state);
        void unregister_object(Property* state);

    virtual ~PropertyList<Property>()
    {
        if (this->objects != NULL)
            free(this->objects);
    }

    PropertyList<Property>()
    : objects(NULL), max(0), ct(0)
    {}
    
    explicit PropertyList<Property>(int max)
    : objects(NULL), max(max), ct(0)
    {
        this->allocate();
    }
};
