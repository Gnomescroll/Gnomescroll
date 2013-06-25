#include "entity_list.hpp"

#include <entity/constants.hpp>
#include <entity/entity/entity.hpp>
#include <entity/components/health.hpp>
#include <entity/entity/entity_data_list.hpp>

namespace Entities
{

EntityID EntityList::get_free_id(EntityType type)
{
    IF_ASSERT(!isValid(type)) return this->null_id;
    if (this->entities[type] == NULL) return this->null_id;
    int max = this->maximums[type];
    int last_free_id = this->last_free_ids[type];
    for (int i=0; i<max; i++)
    {
        EntityID j = EntityID((i + last_free_id) % max);
        if (this->entities[type][j].id == this->null_id)
        {
            this->last_free_ids[type] = (j + 1) % max;
            return j;
        }
    }
    return this->null_id;
}

inline int EntityList::count(EntityType type) const
{
    IF_ASSERT(!isValid(type)) return 0;
    return this->counts[type];
}

inline int EntityList::max(EntityType type) const
{
    IF_ASSERT(!isValid(type)) return 0;
    return this->maximums[type];
}

inline bool EntityList::empty(EntityType type) const
{
    return (this->count(type) == 0);
}

inline bool EntityList::full(EntityType type) const
{
    return (this->count(type) >= this->max(type));
}

inline bool EntityList::in_use(EntityType type, EntityID id) const
{
    IF_ASSERT(!isValid(type)) return true;
    IF_ASSERT(this->maximums == NULL) return true;
    IF_ASSERT(!this->is_valid_id(id, type)) return true;
    return (this->entities[type][id].id != this->null_id);
}

inline bool EntityList::is_valid_id(EntityID id, EntityType type) const
{
    return (id >= 0 && id < this->max(type));
}

void EntityList::destroy(EntityType type, EntityID id)
{
    IF_ASSERT(!isValid(type)) return;
    IF_ASSERT(this->entities[type][id].id == this->null_id) return;
    this->entities[type][id].release_components();
    this->entities[type][id].id = this->null_id;
    this->counts[type] -= 1;
}

Entity* EntityList::get(EntityType type, EntityID id) const
{
    IF_ASSERT(!isValid(type)) return NULL;
    IF_ASSERT(!this->is_valid_id(id, type)) return NULL;

    if (this->entities[type] == NULL) return NULL;
    if (this->entities[type][id].id == this->null_id) return NULL;
    return &this->entities[type][id];
}

Entity* EntityList::create(EntityType type)
{
    return this->create(type, this->get_free_id(type));
}

Entity* EntityList::create(EntityType type, EntityID id)
{
    IF_ASSERT(!isValid(type)) return NULL;
    IF_ASSERT(!this->is_valid_id(id, type)) return NULL;
    IF_ASSERT(this->entities[type][id].id != this->null_id) return NULL;
    this->counts[type] += 1;
    this->entities[type][id].id = id;
    entity_data->load_properties(&this->entities[type][id]);
    return &this->entities[type][id];
}

Entity* EntityList::get_entities(EntityType type) const
{
    IF_ASSERT(!isValid(type)) return NULL;
    return this->entities[type];
}

void EntityList::destroy_all()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        if (this->entities[i] != NULL)
            for (int j=0; j<this->maximums[j]; j++)
                if (this->entities[i][j].id != this->null_id)
                    this->destroy(this->entities[i][j].type, this->entities[i][j].id);
        this->counts[i] = 0;
        this->last_free_ids[i] = 0;
    }
}

void EntityList::set_entity_max(EntityType type, int max)
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    GS_ASSERT(this->maximums[type] == 0);
    GS_ASSERT(this->entities[type] == NULL);
    IF_ASSERT(max < 0) max = 0;
    this->maximums[type] = max;
    if (max > 0)
        this->entities[type] = (Entity*)calloc(max, sizeof(**this->entities));
}

void EntityList::init_entities(EntityDataList* data)
{   // preallocate component pointer buffers
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        size_t n_components = data->get_component_count(EntityType(i));
        for (int j=0; j<this->maximums[i]; j++)
        {
            new (&this->entities[i][j]) Entity(this->null_id);
            this->entities[i][j].init(n_components);
            this->entities[i][j].type = EntityType(i);
        }
    }
}

void EntityList::init()
{
    this->counts = (int*)calloc(MAX_ENTITY_TYPES, sizeof(*this->counts));
    this->maximums = (int*)calloc(MAX_ENTITY_TYPES, sizeof(*this->maximums));
    this->last_free_ids = (int*)calloc(MAX_ENTITY_TYPES, sizeof(*this->last_free_ids));
    this->entities = (Entity**)calloc(MAX_ENTITY_TYPES, sizeof(*this->entities));
}

void EntityList::tick()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        int count = this->counts[i];
        if (count <= 0) continue;
        int max = this->maximums[i];
        Entity* entities = this->entities[i];
        entityTick tick = get_entity_tick_method(EntityType(i));
        if (tick == NULL) continue;
        for (int j=0, k=0; j<max && k<count; j++)
            if (entities[j].id != this->null_id)
            {
                k++;
                tick(&entities[j]);
            }
    }
}

void EntityList::update()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        int count = this->counts[i];
        if (count <= 0) continue;
        int max = this->maximums[i];
        Entity* entities = this->entities[i];
        for (int j=0, k=0; j<max && k<count; j++)
            if (entities[j].id != this->null_id)
            {
                k++;
                entities[j].update();
            }
    }
}

void EntityList::harvest()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        int count = this->counts[i];
        if (count <= 0) continue;
        for (int j=0, k=0; j<this->maximums[i] && k<count; j++)
        {
            if (this->entities[i][j].id == this->null_id)
                continue;
            k++;
            Entity* obj = &this->entities[i][j];
            auto health = GET_COMPONENT_INTERFACE(Health, obj);
            if (health != NULL && health->is_dead())
                Entities::destroy(obj);
        }
    }
}

void EntityList::send_to_client(EntityType type, ClientID client_id) const
{
    GS_ASSERT(type < MAX_ENTITY_TYPES);
    GS_ASSERT(type >= 0);
    if (this->empty(type)) return;
    Entity* entities = this->get_entities(type);
    GS_ASSERT(entities != NULL);
    int count = this->count(type);
    int max = this->max(type);
    GS_ASSERT(max > 0);

    for (int i=0, j=0; i<max && j<count; i++)
        if (entities[i].id != this->null_id)
        {
            j++;
            entities[i].sendToClientCreate(client_id);
        }
}

EntityList::~EntityList()
{
    if (this->entities != NULL)
        for (int i=0; i<MAX_ENTITY_TYPES; i++)
        {
            if (this->maximums != NULL)
                for (int j=0; j<this->maximums[i]; j++)
                    this->entities[i][j].~Entity();
            free(this->entities[i]);
        }
    free(this->entities);

    free(this->counts);
    free(this->maximums);
    free(this->last_free_ids);
}

} // Entities
