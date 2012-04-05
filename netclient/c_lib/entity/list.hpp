#pragma once

#include <c_lib/entity/policy.hpp>

//forward decl
class GameObject_list;
void init_gameobject_list_maximums(GameObject_list* list);

const int MAX_OBJECT_TYPE = 256;
const int MAX_OBJECT_FILTER = 256;

ObjectPolicyInterface* create_object_of_type(Object_types type, int id);

class GameObject_list
{
    protected:
        int max_objects;
        int* index_start;
        int* occupancy;
        int* max_occupancy;
        ObjectPolicyInterface*** objects;

        int max_filtered;

        void teardown()
        {
            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);

            if (this->occupancy != NULL)
                free(this->occupancy);
            if (this->max_occupancy != NULL)
                free(this->max_occupancy);

            if (this->index_start != NULL)
                free(this->index_start);

            if (this->objects != NULL)
            {
                for (int i=0; i<this->max_objects; i++)
                    if (this->objects[i] != NULL)
                    {
                        for (int j=0; j<this->max_occupancy[j]; j++)
                            if (this->objects[i][j] != NULL)
                                delete this->objects[i][j];
                        free(this->objects[i]);
                    }
                free(this->objects);
            }
        }

        void init()
        {
            this->occupancy = (int*)calloc(this->max_objects, sizeof(int));
            this->max_occupancy = (int*)calloc(this->max_objects, sizeof(int));
            init_gameobject_list_maximums(this);

            this->index_start = (int*)calloc(this->max_objects, sizeof(int));

            this->objects = (ObjectPolicyInterface***)calloc(this->max_objects, sizeof(ObjectPolicyInterface**));
            for (int i=0; i<this->max_objects; i++)
                if (this->max_occupancy[i] > 0)
                    this->objects[i] = (ObjectPolicyInterface**)calloc(this->max_occupancy[i], sizeof(ObjectPolicyInterface*));

            this->filtered_objects = (ObjectPolicyInterface**)calloc(this->max_filtered, sizeof(ObjectPolicyInterface*));
            this->filtered_object_distances = (float*)malloc(sizeof(float) * this->max_filtered);
        }

    public:
        const char* name() { return "GameObject"; }

        void set_max_occupancy(Object_types type, int max_allowed)
        {
            if (this->max_occupancy == NULL) return;
            if (type < 0 || type >= this->max_objects)
            {
                printf("WARNING: GameObject_list::set_max_occupancy() -- object type %d out of range\n", type);
                return;
            }
            this->max_occupancy[type] = max_allowed;
        }

        void tick();
        void update();
        bool full(Object_types type);

        int get_index_start(Object_types type)
        {
            return this->index_start[type];
        }
        int get_object_max(Object_types type)
        {
            return this->max_occupancy[type];
        }
        ObjectPolicyInterface** get_object_array(Object_types type)
        {
            return this->objects[type];
        }

        ObjectPolicyInterface* get(Object_types type, int id);

        ObjectPolicyInterface* create(Object_types type);
        ObjectPolicyInterface* create(Object_types type, float x, float y, float z);
        ObjectPolicyInterface* create(Object_types type, float x, float y, float z, float mx, float my, float mz);
        ObjectPolicyInterface* create(Object_types type, int id);
        ObjectPolicyInterface* create(Object_types type, int id, float x, float y, float z);
        ObjectPolicyInterface* create(Object_types type, int id, float x, float y, float z, float mx, float my, float mz);

        void destroy(Object_types type, int id);

        void send_to_client(Object_types type, int client_id);

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
        bool point_occupied_by_type(Object_types type, int x, int y, int z);
        int all_objects_within_sphere(float x, float y, float z, float radius);
        int objects_within_sphere(const Object_types type, float x, float y, float z, float radius);
        int objects_within_sphere(const Object_types* types, const int n_types, float x, float y, float z, float radius);

        ~GameObject_list()
        {
            this->teardown();
        }
        GameObject_list()
        :
        max_objects(MAX_OBJECT_TYPE),
        index_start(0),
        occupancy(NULL), max_occupancy(NULL),
        objects(NULL),
        max_filtered(MAX_OBJECT_FILTER),
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
    list->set_max_occupancy(OBJ_TYPE_DIRT, ItemDrops::DIRT_MAX);
    list->set_max_occupancy(OBJ_TYPE_STONE, ItemDrops::STONE_MAX);
}
