#pragma once

namespace Entities
{

//forward decl
class Entity;
class EntityDataList;

/* Management */

class EntityList
{
    private:
        // keep a counters for individual object indices
        int* indices;
        int* maximums;
        char** used;
        Entity** staging_objects;
        Entity*** objects;

        int get_free_id(EntityType type);

    public:

        // misc
        void set_object_id(Entity* object);
        void set_object_id(Entity* object, int id);

        // count accessors
        inline int count(EntityType type);
        inline int max(EntityType type);
        inline bool empty(EntityType type);
        inline bool full(EntityType type);
        inline bool in_use(EntityType type, int id);

        // object accessors
        Entity* get(EntityType type, int id);
        void destroy(EntityType type, int id);
        Entity* create(EntityType type);
        Entity* create(EntityType type, int id);

        // objects accessors
        Entity** get_objects(EntityType type);
        char* get_used(EntityType type);

        void destroy_all();

        // initializers
        void set_object_max(EntityType type, int max);
        void load_object_data(EntityDataList* data);
        void init();

        // standard tickers
        void tick();    // physics tick
        void update();  // draw state update
        void harvest(); // kill

        // network iterators
        void send_to_client(EntityType type, ClientID client_id);

    ~EntityList();

    EntityList() :
        indices(NULL), maximums(NULL), staging_objects(NULL), objects(NULL)
    {}
};

} // Entities
