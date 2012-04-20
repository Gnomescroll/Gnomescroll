#pragma once

namespace Objects
{

//forward decl
class Object;

/* Management */

const int MAX_OBJECT_TYPES = 256;
class ObjectList
{
    private:
        // keep a counters for individual object indices
        int* indices;
        int* maximums;
        Object*** objects;
        
        int get_free_id(ObjectType type);

    public:

        // misc
        void set_object_id(Object* object, int id);

        // count accessors
        int count(ObjectType type);
        int max(ObjectType type);
        bool empty(ObjectType type);
        bool full(ObjectType type);

        // object accessors
        Object* get(ObjectType type, int id);
        void destroy(ObjectType type, int id);
        Object* create(ObjectType type);
        Object* create(ObjectType type, int id);

        // objects accessors
        Object** get_objects(ObjectType type);

        // initializers
        void set_object_max(ObjectType type, int max);
        void init();

        // standard tickers
        void tick();    // physics tick
        void update();  // draw state update
        void harvest(); // kill
        
    ~ObjectList();

    ObjectList()
    : indices(NULL), maximums(NULL), objects(NULL)
    {}

};

} // Objects
