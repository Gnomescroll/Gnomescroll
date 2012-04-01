#pragma once

#include <c_lib/entity/policy.hpp>

//forward decl
class GameObject_list;
void init_gameobject_list_maximums(GameObject_list* list);

const int MAX_OBJECT_TYPE = 256;

class GameObject_list: public Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>
{
    private:
        int* occupancy;
        int* max_occupancy;

        void teardown()
        {
            if (this->occupancy != NULL)
                free(this->occupancy);
            if (this->max_occupancy != NULL)
                free(this->max_occupancy);
        }

        void init()
        {
            this->occupancy = (int*)calloc(MAX_OBJECT_TYPE, sizeof(int));
            this->max_occupancy = (int*)calloc(MAX_OBJECT_TYPE, sizeof(int));
            init_gameobject_list_maximums(this);
        }

    public:
        const char* name() { return "GameObject"; }

        void set_max_occupancy(Object_types type, int max_allowed)
        {
            if (this->max_occupancy == NULL) return;
            if (type < 0 || type >= MAX_OBJECT_TYPE)
            {
                printf("WARNING: GameObject_list::set_max_occupancy() -- object type %d out of range\n", type);
                return;
            }
            this->max_occupancy[type] = max_allowed;
        }

        bool full(Object_types type);
        void tick();
        void draw();
        void update();

        ObjectPolicyInterface* create(float x, float y, float z, float mx, float my, float mz, Object_types type);
        ObjectPolicyInterface* create(int id, float x, float y, float z, float mx, float my, float mz, Object_types type);
        void destroy(int id);

        void send_to_client(Object_types type, int client_id);
        void alter_owner(int owner, int new_owner);
        bool point_occupied_by_type(Object_types type, int x, int y, int z);
        int objects_within_sphere(float x, float y, float z, float radius);

        /* MAJOR TODO -- MAKE THIS ITS OWN LIST */
        bool team_spawner_available(int team);
        int get_random_spawner(int team);
        int get_numbered_team_spawner(int team, int id);
        ObjectPolicyInterface* get_by_team_index(int team, int team_index);
        bool spawner_exists(int team, int team_index);
        void assign_team_index(ObjectPolicyInterface* spawner);
        /* END SHIT */

        ~GameObject_list()
        {
            this->teardown();
        }
        GameObject_list()
        : occupancy(NULL), max_occupancy(NULL)
        {
            this->init();
            print();
        }
};

// TODO: restructure the list creation to adapt based on type

void init_gameobject_list_maximums(GameObject_list* list)
{
    if (list == NULL)
    {
        printf("WARNING: init_gameobject_list_maximums() -- list is NULL\n");
        return;
    }
    list->set_max_occupancy(OBJ_TYPE_SPAWNER, SPAWNER_MAX);
    list->set_max_occupancy(OBJ_TYPE_TURRET, TURRET_MAX);
    list->set_max_occupancy(OBJ_TYPE_GRENADE_REFILL, ItemDrops::GRENADE_REFILL_MAX);
    list->set_max_occupancy(OBJ_TYPE_LASER_REFILL, ItemDrops::LASER_REFILL_MAX);
}
