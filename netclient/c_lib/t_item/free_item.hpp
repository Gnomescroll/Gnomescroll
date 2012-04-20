#pragma once

#ifdef DC_CLIENT
#include <c_lib/common/gl_assert.hpp>
#include <c_lib/t_item/client/texture.hpp>
#endif

#ifdef DC_SERVER
    #include <c_lib/t_item/net/StoC.hpp>
#endif

#include <c_lib/physics/verlet_particle.hpp>
//#include <c_lib/objects/components/spatial/components.hpp>

namespace t_item
{

using VerletParticle::VerletParticle;

const int FREE_ITEM_TTL = 300; // 10 seconds
const float FREE_ITEM_DAMPENING = 0.5;

typedef enum
{
    type_NULL = 255,
} T_ItemTypes;

class Free_item //: public VerletComponent
{
    private:
    public:
        VerletParticle verlet;

        int id;
        int type;

        int ttl;

    void die();
        
    void tick()
    {
        //this->verlet_bounce(this->damp);
        verlet.bounce_box(0.20);
        this->ttl--;
    }

    void draw();

    Free_item(int id)
    :
    id(id), type(type_NULL),
    ttl(FREE_ITEM_TTL)
    {
        verlet.dampening = FREE_ITEM_DAMPENING;
    }

    void init(float x, float y, float z, float mx, float my, float mz)
    {
        type = rand() % 16;

        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);
    }
};

void Free_item::draw()
{
#ifdef DC_CLIENT
    const float scale = 0.25;
    const float h = 0.35;

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

    int texture_index = this->type;

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(texture_index%8)* (1.0/8.0);
    tx_max = tx_min + (1.0/8.0);
    ty_min = (float)(texture_index/8)* (1.0/8.0);
    ty_max = ty_min + (1.0/8.0);

    Vec3 position = verlet.position;
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

#endif
}

}

/*
    List Definition
*/

#include <c_lib/template/object_list.hpp>

#ifdef DC_SERVER
#include <c_lib/state/server_state.hpp>
#endif

namespace t_item
{

const int FREE_ITEM_MAX = 1024;

class Free_item_list: public Object_list<Free_item, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }
    public:
        Free_item_list() { print(); }

        void draw();
        void tick();

        void check_item_pickups();
};

void Free_item_list::draw()
{
#ifdef DC_CLIENT
    glColor3ub(255,255,255);

    GL_ASSERT(GL_TEXTURE_2D, true);
    GL_ASSERT(GL_DEPTH_TEST, true);

    glEnable(GL_ALPHA_TEST);

    glAlphaFunc ( GL_GREATER, 0.5);

    glBindTexture(GL_TEXTURE_2D, ItemSheetTexture);

    glBegin(GL_QUADS);

    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->draw();

    glEnd();

    glDisable(GL_ALPHA_TEST);


#endif
}

void Free_item_list::tick()
{
    Free_item* free_item;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        free_item = this->a[i];

        free_item->tick();
        if (free_item->ttl <= 0)
        {
            #ifdef DC_SERVER
                free_item->die();
                this->destroy(free_item->id);
            #endif
        }
    }
}


void Free_item_list::check_item_pickups()
{
#ifdef DC_SERVER
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        Free_item* free_item = this->a[i];

        const static float pick_up_distance = 0.5;
        Agent_state* agent = nearest_agent_in_range(free_item->verlet.position, pick_up_distance);

        if(agent == NULL) continue;

        printf("agent %i picked up item %i \n", agent->id, free_item->id);

        free_item_picked_up_StoC p;
        p.id = free_item->id;
        p.agent_id = agent->id;
        p.broadcast();

        t_item::free_item_list->destroy(free_item->id);


        /*
            Put item in agent inventory
        */
        assert(AgentInventoryList[agent->id] != NO_AGENT);

        int inventory_id = AgentInventoryList[agent->id];
        ItemContainer* ic = item_container_list.get(inventory_id)
        
        if(ic == 0)
        {
            printf("Free_item_list::check_item_pickups, item container null \n");
            return;
        }

        int slot = ic->get_empty_slot();
        if(slot == -1)
        {
            printf("Free_item_list::check_item_pickups, Agent inventory full: item deleted, fix \n");
            return;
        }

        int item_type = rand()%12;

        int slot = ic->create_item(item_type);   //insert item on server

        class item_create_StoC p;
        p.item_id = item_type;
        p.item_type = item_type;
        p.inventory_id = inventory_id;
        p.inventory_slot = slot;

        p.sendToClient(agent->id); //warning, assumes agent and player id are same

/*
item_create_StoC: public FixedSizeReliableNetPacketToClient<item_create_StoC>
{
    public:
        //uint8_t type;
        uint16_t item_id;
        uint16_t item_type;
        uint16_t inventory_id;
        uint16_t inventory_slot;

}
*/
#endif

}


/*
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
    ServerState::agent_list->filtered_objects[0]->id;
*/


}
