#include "item_particle.hpp"

#include <item/particle/net/StoC.hpp>
#include <item/_interface.hpp>

#include <item/common/constants.hpp>

namespace ItemParticle
{

#if DC_CLIENT

void ItemParticle::draw()
{    
    const float scale = ITEM_PARTICLE_SPRITE_RENDER_SCALE;
    const float h = 0.35f;

    Vec3 position = quadrant_translate_position(current_camera_position, verlet.position);
    if (!sphere_fulstrum_test(position.x, position.y, position.z+h, scale*2)) return;
    
    Vec3 up = vec3_init(
        model_view_matrix[0]*scale,
        model_view_matrix[4]*scale,
        model_view_matrix[8]*scale
   );
    Vec3 right = vec3_init(
        model_view_matrix[1]*scale,
        model_view_matrix[5]*scale,
        model_view_matrix[9]*scale
   );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->sprite_index%16)* (1.0f/16.0f);
    tx_max = tx_min + (1.0f/16.0f);
    ty_min = (float)(this->sprite_index/16)* (1.0f/16.0f);
    ty_max = ty_min + (1.0f/16.0f);

    Vec3 p = vec3_sub(position, vec3_add(right, up));
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(up, right));
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_add(up, right));
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z+h);

    p = vec3_add(position, vec3_sub(right, up));
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z+h);
}
#endif

void ItemParticle::die()
{
    #if DC_SERVER
    broadcast_particle_item_destroy(this->id);

    GS_ASSERT(this->item_id != NULL_ITEM);
    Item::Item* item = Item::get_item(this->item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL) item->location = IL_NOWHERE;
    
    // destroy source item if we weren't picked up
    if (this->target_agent == NULL_AGENT)
        Item::destroy_item(this->item_id);
    #endif
}

void ItemParticle::tick()
{
    #if DC_SERVER
    GS_ASSERT(!this->get_picked_up || this->target_agent != NULL_AGENT);
    
    this->ttl--;
    if (this->ttl <= 0 && this->target_agent != NULL_AGENT)
        // particle failed to reach target agent in time, reset
        this->pickup_cancelled();
    #endif
    
    // orient to target agent
    if (this->target_agent != NULL_AGENT)
    {
        Agents::Agent* a = Agents::get_agent(this->target_agent);
        if (a != NULL)
        {
            Vec3 p = a->get_center();

            // die if very close
            if (vec3_distance_squared(p, this->verlet.position) < 
              ITEM_PARTICLE_PICKUP_END_DISTANCE*ITEM_PARTICLE_PICKUP_END_DISTANCE)
            {
                #if DC_SERVER
                this->get_picked_up = true;
                this->ttl = 0;
                #endif
            }
            else
            {    // orient towards agent
                Vec3 pos = quadrant_translate_position(p, this->verlet.position);
                if (!vec3_equal(p, pos))
                { 
                    Vec3 direction = vec3_sub(p, pos);
                    normalize_vector(&direction);
                    direction = vec3_scalar_mult(direction, ITEM_PARTICLE_PICKUP_MOMENTUM);
                    this->verlet.velocity = direction;
                }
            }
        }
    }
    
    // dont apply physics if the chunk is not loaded (in client)
    #if DC_CLIENT
    if (t_map::position_is_loaded(this->verlet.position.x, this->verlet.position.y))
        this->verlet.bounce_box();
    #endif

    #if DC_SERVER
    this->verlet.bounce_box();

    if (this->verlet.position.z < OBJECT_DEPTH_MAX) this->ttl = 0;
    this->pickup_prevention--;
    #endif
}

#if DC_CLIENT
void ItemParticle::init(int item_type, float x, float y, float z, float mx, float my, float mz)
#endif
#if DC_SERVER
void ItemParticle::init(ItemID item_id, int item_type, float x, float y, float z, float mx, float my, float mz)
#endif
{
    this->verlet.box_radius = ITEM_PARTICLE_RADIUS;
    this->item_type = item_type;
    #if DC_CLIENT
    this->sprite_index = Item::get_sprite_index_for_type(item_type);
    this->is_voxel = Item::item_type_is_voxel(item_type);
    if (this->is_voxel)
    {
        this->voxel.size = ITEM_PARTICLE_VOXEL_RENDER_SCALE / 2.0f;   // cut in half because v_set2(centered) is [-1,1](2x)
        this->voxel.pixel_width = 32;
        this->voxel.texture_index = Item::get_particle_voxel_texture(item_type);
        this->voxel.cube_type = Item::get_block_type_id(item_type);
        this->voxel.theta = randf();
        this->voxel.init();
    }
    
    this->should_draw = true;
    #endif    
    #if DC_SERVER
    this->item_id = item_id;
    this->pickup_prevention = ITEM_INITIAL_PICKUP_PREVENTION;
    #endif
    ASSERT_BOXED_POINTf(x);
    ASSERT_BOXED_POINTf(y);
    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);

    GS_ASSERT(vec3_is_valid(verlet.position));
    GS_ASSERT(vec3_is_valid(verlet.velocity));
    
    verlet.dampening = ITEM_PARTICLE_DAMPENING;
}

void ItemParticle::picked_up(AgentID agent_id)
{
    GS_ASSERT(this->target_agent == NULL_AGENT);
    this->target_agent = agent_id;
    #if DC_SERVER
    this->ttl = ITEM_PARTICLE_PICKED_UP_TTL;
    broadcast_particle_item_picked_up(agent_id, this->id);
    #endif
}

void ItemParticle::pickup_cancelled()
{
    GS_ASSERT(this->target_agent != NULL_AGENT);
    this->target_agent = NULL_AGENT;
    this->verlet.velocity = vec3_init(0.0f,0.0f,0.0f);
    #if DC_SERVER
    this->ttl = ITEM_PARTICLE_TTL;
    this->get_picked_up = false;
    broadcast_particle_item_pickup_cancelled(this->id);
    #endif
}

ItemParticle::ItemParticle(ItemParticleID id) :
    id(id),
    item_type(NULL_ITEM_TYPE),
    target_agent(NULL_AGENT),
    #if DC_SERVER
    ttl(ITEM_PARTICLE_TTL),
    item_id(NULL_ITEM),
    pickup_prevention(0),
    get_picked_up(false)
    #endif
    #if DC_CLIENT
    is_voxel(false),
    sprite_index(ERROR_SPRITE),
    should_draw(true)
    #endif
{
    verlet.dampening = ITEM_PARTICLE_DAMPENING;
    #if DC_SERVER
    this->broadcast_tick = randrange(0, ITEM_PARTICLE_STATE_BROADCAST_TICK_RATE-1);
    #endif
}


// ItemParticle_list

#if DC_SERVER
static bool pickup_item_particle(ItemParticleID particle_id)
{    // this method takes care of all state updates, including particle destruction
    IF_ASSERT(particle_id == NULL_PARTICLE) return false;

    ItemParticle* particle = get(particle_id);
    IF_ASSERT(particle == NULL) return false;
    IF_ASSERT(particle->target_agent == NULL_AGENT) return false;
    
    Item::Item* item = Item::get_item(particle->item_id);
    IF_ASSERT(item == NULL) return false;

    Agents::Agent* agent = Agents::get_agent(particle->target_agent);
    if (agent == NULL) return false;    
    
    // attempt to transfer item particle to intended destination
    // the particle should automatically add to the client's inventories
    // it was pre-split in the pickup initiation phase
    // since the particles fly and may reach out-of-order, this can fail
    // if this fails, return false, and the particle should be reset to normal
    
    static int coin_type = Item::get_item_type("synthesizer_coin");
    GS_ASSERT(coin_type != NULL_ITEM_TYPE);
    static int energy_tank_type = Item::get_item_type("energy_tank");
    GS_ASSERT(energy_tank_type != NULL_ITEM_TYPE);
    
    // get agent toolbelt and container in array
    int n_containers = 2;
    if (item->type == coin_type || item->type == energy_tank_type)
        n_containers = 3;
    int container_index = 0;
    MALLOX(ItemContainer::ItemContainerInterface*, containers, n_containers);
    memset(containers, 0, n_containers * sizeof(ItemContainer::ItemContainerInterface*));
    
    if (item->type == coin_type)
    {
        ItemContainerID container_id = ItemContainer::get_agent_synthesizer(agent->id);
        GS_ASSERT(container_id != NULL_CONTAINER);
        if (container_id != NULL_CONTAINER)
            containers[container_index++] = ItemContainer::get_container(container_id);
    }
    else
    if (item->type == energy_tank_type)
    {
        ItemContainerID container_id = ItemContainer::get_agent_energy_tanks(agent->id);
        GS_ASSERT(container_id != NULL_CONTAINER);
        if (container_id != NULL_CONTAINER)
            containers[container_index++] = ItemContainer::get_container(container_id);
    }
    
    ItemContainerID container_id = ItemContainer::get_agent_toolbelt(agent->id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id != NULL_CONTAINER)
        containers[container_index++] = ItemContainer::get_container(container_id);

    container_id = ItemContainer::get_agent_inventory(agent->id);
        GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id != NULL_CONTAINER)
        containers[container_index++] = ItemContainer::get_container(container_id);

    // For [<synthesizer>, toolbelt, inventory]
        // A: try to stack with slot, return true
        // B: try to add to empty slot, return true
    // return false

    ItemContainer::ItemContainerInterface* container = NULL;
    // for toolbelt,inventory
    for (int i=0; i<n_containers; i++)
    {
        container = containers[i];
        if (container == NULL) continue;
        
        // try to stack with a slot
        int slot = container->get_stackable_slot(item->type, item->stack_size);
        if (slot != NULL_SLOT)
        {
            ItemID slot_item_id = container->get_item(slot);
            GS_ASSERT(slot_item_id != NULL_ITEM);
            if (slot_item_id != NULL_ITEM)
            {
                Item::merge_item_stack(item->id, slot_item_id);
                Item::destroy_item(item->id);
                Item::send_item_state(slot_item_id);
                return true;
            }
        }        
        
        // try to put in empty slot
        slot = container->get_empty_slot();
        if (slot != NULL_SLOT)
        {
            bool success = ItemContainer::transfer_particle_to_container(
                item->id, particle->id, container->id, slot);
            if (success) return true;
        }
    }

    return false;
}
#endif

void ItemParticle_list::tick()
{
    ItemParticle* ip;
    for (unsigned int i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        ip = &this->objects[i];

        ip->tick();
        
        #if DC_SERVER
        if (ip->get_picked_up)
        {    // attempt to transfer to container
            bool success = pickup_item_particle(ip->id);
            if (!success) // failed to pickup as predicted, reset particle ttl
                ip->pickup_cancelled();
        }
        else if (ip->ttl <= 0)
        {
            ip->die();
            this->destroy(ip->id);
        }
        #endif
    }
}

}    // ItemParticle
