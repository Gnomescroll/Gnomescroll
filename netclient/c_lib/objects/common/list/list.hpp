#pragma once

#include <c_lib/objects/common/interface/policy.hpp>

/* List management */

// inherit this for a property list tracker
class BehaviourList
{
    private:
        virtual const char* name() { return "BehaviourList"; }

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

        void init(int max)
        {
            if (this->objects != NULL)
                return;
            this->max = max;
            this->allocate();
        }

    virtual ~BehaviourList()
    {
        if (this->objects != NULL)
            free(this->objects);
    }

    BehaviourList()
    : objects(NULL), max(0), ct(0)
    {}
    
    explicit BehaviourList(int max)
    : objects(NULL), max(0), ct(0)
    {
        this->init(max);
    }
};


class BehaviourListAggregate
{
    public:
        BehaviourList* lists;
        int n;
        
    void init(int n_lists)
    {
        if (n < 0)
        {
            printf("WARNING: BehaviourListAggregate - init() -- n_lists=%d\n", n_lists);
            return;
        }
        this->lists = new BehaviourList[n_lists];
    }

    void init(int n_lists, int size_of_lists)
    {
        this->init(n_lists);
        if (this->lists != NULL)
            for (int i=0; i<this->n; i++)
                this->lists[i].init(size_of_lists);
    }

    BehaviourList* get(int id)
    {
        if (id < 0 || id >= n)
            return NULL;
        return &this->lists[id];
    }

    ~BehaviourListAggregate()
    {
        if (this->lists != NULL)
            delete[] this->lists;
    }

    BehaviourListAggregate()
    : lists(NULL), n(0)
    {}
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

        void register_object(Property* state)
        {
            if (this->ct >= this->max)
            {
                printf("WARNING: %s is full\n", name());
                return;
            }
            int i=0;
            for (;i<this->max; i++)
            {
                if (this->objects[i] == NULL)
                {
                    state->id = i;
                    this->objects[i] = state;
                    this->ct++;
                    break;
                }
            }
            if (i == this->max)
                printf("WARNING: no empty slots found in %s\n", name());
        }

        void unregister_object(Property* state)
        {
            if (state->id < 0 || state->id >= this->max)
                return;
            if (this->objects[state->id] == NULL) return;
            this->objects[state->id] = NULL;
            this->ct--;
            state->id = -1;
            return;
        }

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

template <class List>
class PropertyListAggregate
{
    public:
        List* lists;
        int n;
        
    void init(int n_lists)
    {
        if (n < 0)
        {
            printf("WARNING: PropertyListAggregate - init() -- n_lists=%d\n", n_lists);
            return;
        }
        this->lists = new List[n_lists];
    }

    void init(int n_lists, int size_of_lists)
    {
        this->init(n_lists);
        if (this->lists != NULL)
            for (int i=0; i<this->n; i++)
                this->lists[i].init(size_of_lists);
    }

    List* get(int id)
    {
        if (id < 0 || id >= n)
            return NULL;
        return &this->lists[id];
    }

    ~PropertyListAggregate<List>()
    {
        if (this->lists != NULL)
            delete[] this->lists;
    }

    PropertyListAggregate<List>()
    : lists(NULL), n(0)
    {}
};
