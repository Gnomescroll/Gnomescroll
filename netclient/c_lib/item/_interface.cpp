#include "_interface.hpp"


#include <item/item_container.hpp>
#include <item/item.hpp>

#include <item/_state.hpp>

#if DC_SERVER
#include <item/net/StoC.hpp>
#endif

namespace Item
{

void init()
{
    item_container_list = new ItemContainerList;
    item_list           = new ItemList;

    set_sprite_ids();

    #if DC_SERVER
    agent_container_list = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list  = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_nanite_list    = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_hand_list      = (int*)malloc(AGENT_MAX * sizeof(int));
    for (int i=0; i<AGENT_MAX; i++) agent_container_list[i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_nanite_list   [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_hand_list     [i] = NULL_ITEM;
    #endif
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;
    if (item_list           != NULL) delete item_list;

    #if DC_SERVER
    if (agent_container_list != NULL) free(agent_container_list);
    if (agent_toolbelt_list  != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list    != NULL) free(agent_nanite_list);
    if (agent_hand_list      != NULL) free(agent_hand_list);
    #endif
}


ItemContainer* get_container(int id)
{
    assert(item_container_list != NULL);
    return item_container_list->get(id);
}

Item* get_item(ItemID id)
{
    return item_list->get(id);
}

int get_item_type(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_ITEM_TYPE;
    return item->type;
}

}
 
// Client
#if DC_CLIENT
namespace Item
{

Item* create_item(int item_type, ItemID item_id)
{
    return item_list->create_type(item_type, item_id);
}

ItemID* get_container_contents(int container_id)
{
    ItemContainer* container = item_container_list->get(container_id);
    if (container == NULL) return NULL;
    return container->slot;
}

}
#endif 

// Server
#if DC_SERVER

namespace Item
{

    
ItemContainer* get_agent_container(int agent_id)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    int container_id = agent_container_list[agent_id];
    return item_container_list->get(container_id);
}

static void assign_container_to_agent(ItemContainer* container, ItemContainerType type, int* container_list, int agent_id, int client_id)
{
    assert(container != NULL);
    assert(container_list[agent_id] == NULL_ITEM);
    container_list[agent_id] = container->id;
    init_container(container, type);
    send_container_create(container, client_id);
    send_container_assign(container, client_id);
}

void assign_containers_to_agent(int agent_id, int client_id)
{
    assert(agent_id >= 0 && agent_id < AGENT_MAX);
    
    ItemContainer* agent_container = item_container_list->create();
    assign_container_to_agent(agent_container, AGENT_CONTAINER, agent_container_list, agent_id, client_id);
    
    ItemContainer* agent_toolbelt = item_container_list->create();
    assign_container_to_agent(agent_toolbelt, AGENT_TOOLBELT, agent_toolbelt_list, agent_id, client_id);
    
    ItemContainer* agent_nanite = item_container_list->create();
    assign_container_to_agent(agent_nanite, AGENT_NANITE, agent_nanite_list, agent_id, client_id);
}


Item* create_item(int item_type)
{
    // TODO -- dont override type
    item_type = randrange(0,7);
    return item_list->create_type(item_type);
}

Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz)
{    
    Item* item = create_item(item_type);
    if (item == NULL) return NULL;
    Particle::ItemParticle* particle = Particle::create_item_particle(item->id, item->type, x,y,z,vx,vy,vz);
    if (particle == NULL) return item;
    Particle::broadcast_particle_item_create(particle->id);
    return item;
}

void check_item_pickups()
{
    using Particle::ItemParticle;
    using Particle::item_particle_list;
    using Particle::item_particle_picked_up_StoC;
    
    for (int i=0; i<item_particle_list->n_max; i++)
    {
        if (item_particle_list->a[i] == NULL) continue;
        ItemParticle* item_particle = item_particle_list->a[i];
        if (!item_particle->can_be_picked_up()) continue;
        Item* item = get_item(item_particle->item_id);
        assert(item != NULL);
    
        const static float pick_up_distance = 1.0f;
        Agent_state* agent = nearest_living_agent_in_range(item_particle->verlet.position, pick_up_distance);
        if (agent == NULL) continue;

        item_particle->picked_up(agent->id);

        ItemContainer* ic = get_agent_container(agent->id);
        if (ic == NULL) return;

        int slot = auto_add_item_to_container(ic, item->id);   //insert item on server
        if (slot == NULL_SLOT) return;

        send_container_item_create(agent->client_id, item->id, ic->id, slot);
    }
}

void throw_item(int agent_id, ItemID item_id)
{
    assert(item_id != NULL_ITEM);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;

    Item* item = get_item(item_id);
    if (item == NULL) return;

    broadcast_item_destroy(a->client_id, item->id);

    Vec3 position = a->get_center();
    float x = position.x;
    float y = position.y;
    float z = position.z;

    const float mom = 2.0f;
    Vec3 force = a->s.forward_vector();
    force.z = 0;
    normalize_vector(&force);
    force = vec3_scalar_mult(force, mom);
    force = vec3_bias(force, (randf()-0.5f) * 30);
    float vx = force.x;
    float vy = force.y;
    float vz = force.z;

    // create particle
    Particle::ItemParticle* particle = Particle::create_item_particle(item->id, item->type, x,y,z,vx,vy,vz);
    if (particle == NULL) return;
    Particle::broadcast_particle_item_create(particle->id);
    particle->lock_pickup();
}

}

#endif 
