#pragma once

#include <c_lib/entity/policy.hpp>

//forward decl
class GameObject_list;
void init_gameobject_list_maximums(GameObject_list* list);

const int MAX_OBJECT_TYPE = 256;

ObjectPolicyInterface* create_object_of_type(Object_types type, int id);

class GameObject_list
{
    protected:
        int id_start;
        int* occupancy;
        int* max_occupancy;

        void teardown()
        {
            if (this->a != NULL)
                free(this->a);

            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);

            if (this->occupancy != NULL)
                free(this->occupancy);
            if (this->max_occupancy != NULL)
                free(this->max_occupancy);
        }

        void init()
        {
            this->a = (ObjectPolicyInterface**)calloc(this->max, sizeof(ObjectPolicyInterface*));

            this->filtered_objects = (ObjectPolicyInterface**)calloc(this->max, sizeof(ObjectPolicyInterface*));
            this->filtered_object_distances = (float*)malloc(sizeof(float) * this->max);

            this->occupancy = (int*)calloc(MAX_OBJECT_TYPE, sizeof(int));
            this->max_occupancy = (int*)calloc(MAX_OBJECT_TYPE, sizeof(int));
            init_gameobject_list_maximums(this);
        }

    public:
        int max;
        int ct;
        ObjectPolicyInterface** a;
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

        void tick();
        void draw();
        void update();
        bool full(Object_types type);

        ObjectPolicyInterface* get(Object_types type, int id);

        ObjectPolicyInterface* create(Object_types type);
        ObjectPolicyInterface* create(Object_types type, float x, float y, float z);
        ObjectPolicyInterface* create(Object_types type, float x, float y, float z, float mx, float my, float mz);
        ObjectPolicyInterface* create(Object_types type, int id);
        ObjectPolicyInterface* create(Object_types type, int id, float x, float y, float z);
        ObjectPolicyInterface* create(Object_types type, int id, float x, float y, float z, float mx, float my, float mz);

        void destroy(Object_types type, int id);

        void send_to_client(Object_types type, int client_id);
        void transfer_ownership(int owner, int new_owner);
        bool point_occupied_by_type(Object_types type, int x, int y, int z);
        int objects_within_sphere(float x, float y, float z, float radius);

        void print()
        {
            const char* n = this->name();
            printf("%s_list instantiated at %p\n", n, this);
        }

        // TODO -- objects should track themselves in this list
        // as items go in and out of inventory
        // filtering
        ObjectPolicyInterface** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        int n_filtered;

        ~GameObject_list()
        {
            this->teardown();
        }
        GameObject_list()
        :
        id_start(0),
        occupancy(NULL), max_occupancy(NULL),
        max(GAME_OBJECTS_MAX), ct(0),
        filtered_objects(NULL), filtered_object_distances(NULL), n_filtered(0)
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
