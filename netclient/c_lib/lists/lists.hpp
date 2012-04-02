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

    void register_object(ListProperties* state);
    void unregister_object(ListProperties* state);

    ~BehaviourList()
    {}
    explicit BehaviourList(int max)
    : objects(NULL), max(max), ct(0)
    {}
};
