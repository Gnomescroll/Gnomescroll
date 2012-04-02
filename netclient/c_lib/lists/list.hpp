#pragma once

/* List management */

// inherit this into a property object for tracking
class ListProperties
{
    public:
    int id;
    ObjectPolicyInterface* obj;
    void* list;
    
    ListProperties()
    : id(-1), obj(NULL), list(NULL)
    {}
};

// inherit this for a property list tracker
class BehaviourList
{
    private:
        virtual const char* name() = 0;
    public:
        ListProperties** objects;
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
            this->objects = (ListProperties**)calloc(this->max, sizeof(ListProperties*));
        else
            printf("WARNING: BehaviourList::allocate() -- attempt to allocate more than once\n");
    }

    void register_object(ListProperties* state);
    void unregister_object(ListProperties* state);

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
