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
        
    public:
        int get_free_id(ObjectType type);
        void destroy(ObjectType type, int id);
        Object* create(ObjectType type, int id);
        void set_object_max(ObjectType type, int max);
        void init();

    ~ObjectList();

    ObjectList()
    : indices(NULL), maximums(NULL), objects(NULL)
    {}

};

} // Objects
