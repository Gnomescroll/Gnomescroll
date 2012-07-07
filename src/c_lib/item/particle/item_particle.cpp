#include "item_particle.hpp"

#include <item/particle/net/StoC.hpp>
#include <item/_interface.hpp>

#include <item/common/constant.hpp>

namespace ItemParticle
{

#if DC_CLIENT

GLuint ItemSheetTexture = 0;

void init_item_particle()
{
    SDL_Surface* s = TextureSheetLoader::ItemSurface;

    if (s == NULL)
    {
        printf("ItemParticle::init_item_particle, error \n");
        return;
    }
    GS_ASSERT(s != NULL);

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemSheetTexture );

    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //GLenum internalFormat = 4;
    GLenum internalFormat = 4;
    GLenum format = GL_BGRA;
    if (s->format->Rmask == 0x000000ff)
        format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
    
    glDisable(GL_TEXTURE_2D);
}

void teardown_item_particle()
{
    glDeleteTextures(1, &ItemSheetTexture );
}

void ItemParticle::draw()
{
    Vec3 position = quadrant_translate_position(current_camera_position, verlet.position);
    if (point_fulstrum_test(position.x, position.y, position.z) == false) return;

    const float scale = 0.25f;
    const float h = 0.35f;

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
    if (this->target_agent == NO_AGENT)
		Item::destroy_item(this->item_id);
    #endif
}

void ItemParticle::tick()
{
	#if DC_SERVER
	GS_ASSERT(!this->get_picked_up || this->target_agent != NO_AGENT);
	
	this->ttl--;
	if (this->ttl <= 0 && this->target_agent != NO_AGENT)
		// particle failed to reach target agent in time, reset
		this->pickup_cancelled();
	#endif
	
	// orient to target agent
	if (this->target_agent != NO_AGENT)
	{
		Agent_state* a = STATE::agent_list->get(this->target_agent);
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
			{	// orient towards agent
				Vec3 direction = vec3_sub(p, this->verlet.position);
				normalize_vector(&direction);
				direction = vec3_scalar_mult(direction, ITEM_PARTICLE_PICKUP_MOMENTUM);
				this->verlet.velocity = direction;
			}
		}
	}
	
	verlet.bounce_box(ITEM_PARTICLE_RADIUS);
	#if DC_SERVER
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
    this->item_type = item_type;
    #if DC_CLIENT
    this->sprite_index = Item::get_sprite_index_for_type(item_type);
    this->is_voxel = Item::item_type_is_voxel(item_type);
    if (this->is_voxel)
    {
        this->voxel.size = 0.25f;
        this->voxel.pixel_width = 32;
        this->voxel.texture_index = Item::get_particle_voxel_texture(item_type);
        this->voxel.init();
    }
    #endif    
    #if DC_SERVER
    this->item_id = item_id;
    #endif
    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);
    verlet.dampening = ITEM_PARTICLE_DAMPENING;
}

void ItemParticle::picked_up(int agent_id)
{
	GS_ASSERT(this->target_agent == NO_AGENT);
	this->target_agent = agent_id;
	#if DC_SERVER
	this->ttl = ITEM_PARTICLE_PICKED_UP_TTL;
	broadcast_particle_item_picked_up(agent_id, this->id);
	#endif
}

void ItemParticle::pickup_cancelled()
{
	GS_ASSERT(this->target_agent != NO_AGENT);
	this->target_agent = NO_AGENT;
	this->verlet.velocity = vec3_init(0.0f,0.0f,0.0f);
	#if DC_SERVER
	this->ttl = ITEM_PARTICLE_TTL;
	this->get_picked_up = false;
	broadcast_particle_item_pickup_cancelled(this->id);
	#endif
}

ItemParticle::ItemParticle(int id) :
    id(id),
    item_type(NULL_ITEM_TYPE),
    target_agent(NO_AGENT),
    #if DC_SERVER
    ttl(ITEM_PARTICLE_TTL),
    item_id(NULL_ITEM),
    pickup_prevention(0),
    get_picked_up(false)
    #endif
    #if DC_CLIENT
    is_voxel(false),
    sprite_index(ERROR_SPRITE)
    #endif
{
    verlet.dampening = ITEM_PARTICLE_DAMPENING;
    #if DC_SERVER
    this->broadcast_tick = randrange(0, ITEM_PARTICLE_STATE_BROADCAST_TICK_RATE-1);
    #endif
}


// ItemParticle_list

#if DC_SERVER
static bool pickup_item_particle(int particle_id)
{	// this method takes care of all state updates, including particle destruction
	
	GS_ASSERT(particle_id != NULL_PARTICLE);
	if (particle_id == NULL_PARTICLE) return false;

	ItemParticle* particle = get(particle_id);
	GS_ASSERT(particle != NULL);
	if (particle == NULL) return false;
	
	GS_ASSERT(particle->target_agent != NO_AGENT);
	if (particle->target_agent == NO_AGENT) return false;
	
	Item::Item* item = Item::get_item(particle->item_id);
	GS_ASSERT(item != NULL);
	if (item == NULL) return false;

	Agent_state* agent = ServerState::agent_list->get(particle->target_agent);
	if (agent == NULL) return false;	
	
	// attempt to transfer item particle to intended destination
	// the particle should automatically add to the client's inventories
	// it was pre-split in the pickup initiation phase
	// since the particles fly and may reach out-of-order, this can fail
	// if this fails, return false, and the particle should be reset to normal
	
	// get agent toolbelt and container in array
    ItemContainer::ItemContainerInterface* containers[2] = {NULL,NULL};
	int container_id = ItemContainer::get_agent_toolbelt(agent->id);
	if (container_id != NULL_CONTAINER)
		containers[0] = ItemContainer::get_container(container_id);
	container_id = ItemContainer::get_agent_container(agent->id);
	if (container_id != NULL_CONTAINER)
		containers[1] = ItemContainer::get_container(container_id);

	// For [toolbelt, inventory]
		// A: try to stack with slot, return true
		// B: try to add to empty slot, return true
	// return false

	ItemContainer::ItemContainerInterface* container = NULL;
	// for toolbelt,inventory
	for (int i=0; i<2; i++)
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
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        ip = this->a[i];

        ip->tick();
        #if DC_SERVER
        if (ip->get_picked_up)
        {	// attempt to transfer to container
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

}	// ItemParticle
