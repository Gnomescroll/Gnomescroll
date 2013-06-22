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
        // keep a counters for individual entity counts
        int* counts;
        int* maximums;
        int* last_free_ids;
        Entity** entities;

        EntityID get_free_id(EntityType type);

    public:

        static const EntityID null_id = NULL_ENTITY;

    // sanity checks
    inline bool is_valid_id(EntityID id, EntityType type) const;

    // count accessors
    inline int count(EntityType type) const;
    inline int max(EntityType type) const;
    inline bool empty(EntityType type) const;
    inline bool full(EntityType type) const;
    inline bool in_use(EntityType type, EntityID id) const;

    // entity accessors
    Entity* get(EntityType type, EntityID id) const;
    void destroy(EntityType type, EntityID id);
    Entity* create(EntityType type);
    Entity* create(EntityType type, EntityID id);

    // entities accessors
    Entity* get_entities(EntityType type) const;

    void destroy_all();

    // initializers
    void set_entity_max(EntityType type, int max);
    void init_entities(EntityDataList* data);
    void init();

    // standard tickers
    void tick();    // physics tick
    void update();  // draw state update
    void harvest(); // kill

    // network iterators
    void send_to_client(EntityType type, ClientID client_id) const;

    ~EntityList();

    EntityList() :
        counts(NULL), maximums(NULL), last_free_ids(NULL), entities(NULL)
    {}
};

} // Entities
