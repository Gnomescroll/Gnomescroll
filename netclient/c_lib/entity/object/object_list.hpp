#pragma once

namespace Objects
{

//forward decl
class Object;
class ObjectDataList;

/* Management */

const int MAX_OBJECT_TYPES = 256;
class ObjectList
{
    private:
        // keep a counters for individual object indices
        int* indices;
        int* maximums;
        char** used;
        Object*** objects;
        
        int get_free_id(ObjectType type);

    public:

        // misc
        void set_object_id(Object* object);
        void set_object_id(Object* object, int id);

        // count accessors
        inline int count(ObjectType type);
        inline int max(ObjectType type);
        inline bool empty(ObjectType type);
        inline bool full(ObjectType type);

        // object accessors
        Object* get(ObjectType type, int id);
        void destroy(ObjectType type, int id);
        Object* create(ObjectType type);
        Object* create(ObjectType type, int id);

        // objects accessors
        Object** get_objects(ObjectType type);
        char* get_used(ObjectType type);

        // initializers
        void set_object_max(ObjectType type, int max);
        void load_object_data(ObjectDataList* data);
        void init();

        // standard tickers
        void tick();    // physics tick
        void update();  // draw state update
        void harvest(); // kill

        // network iterators
        void send_to_client(ObjectType type, int client_id);
        
    ~ObjectList();

    ObjectList()
    : indices(NULL), maximums(NULL), objects(NULL)
    {}
};

} // Objects
