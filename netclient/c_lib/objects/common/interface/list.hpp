#pragma once

#include <c_lib/objects/common/interface/policy.hpp>

//forward decl
class GameObject_list;
void init_gameobject_list_maximums(GameObject_list* list);

const int MAX_OBJECT_TYPE = 256;
const int MAX_OBJECT_FILTER = 256;

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
            if (this->index_start != NULL)
                free(this->index_start);

            if (this->objects != NULL)
            {
                for (int i=0; i<this->max_objects; i++)
                {
                    ObjectPolicyInterface** list = this->objects[i];
                    if (list == NULL) continue;
                    if (this->max_occupancy[i] <= 0) continue;
                    for (int j=0; j<this->max_occupancy[i]; j++)
                        if (list[j] != NULL)
                            delete list[j];
                    free(list);
                }
                free(this->objects);
            }

            if (this->occupancy != NULL)
                free(this->occupancy);
            if (this->max_occupancy != NULL)
                free(this->max_occupancy);

            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);
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

        void set_max_occupancy(ObjectType type, int max_allowed)
        {
            if (this->max_occupancy == NULL) return;
            if (type < 0 || type >= this->max_objects)
            {
                printf("WARNING: GameObject_list::set_max_occupancy() -- object type %d out of range\n", type);
                return;
            }
            if (this->max_occupancy[type] != 0)
                printf("WARNING: GameObject_list::set_max_occupancy() -- max_occupancy already set to %d\n", this->max_occupancy[type]);
            assert(this->max_occupancy[type] == 0);
            this->max_occupancy[type] = max_allowed;
        }

        void tick();
        void update();
        bool full(ObjectType type);

        int get_index_start(ObjectType type)
        {
            if (type < 0 || type >= MAX_OBJECT_TYPE)
                return 0;
            return this->index_start[type];
        }
        int get_object_count(ObjectType type)
        {
            if (type < 0 || type >= MAX_OBJECT_TYPE)
                return 0;
            return this->occupancy[type];
        }
        int get_object_max(ObjectType type)
        {
            if (type < 0 || type >= MAX_OBJECT_TYPE)
                return 0;
            return this->max_occupancy[type];
        }
        ObjectPolicyInterface** get_object_array(ObjectType type)
        {
            if (type < 0 || type >= MAX_OBJECT_TYPE)
                return NULL;
            return this->objects[type];
        }

        ObjectPolicyInterface* get(ObjectType type, int id);

        ObjectPolicyInterface* create(ObjectType type);
        ObjectPolicyInterface* create(ObjectType type, int id);

        void destroy(ObjectType type, int id);

        void send_to_client(ObjectType type, int client_id);

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
        bool point_occupied_by_type(ObjectType type, int x, int y, int z);
        int all_objects_within_sphere(float x, float y, float z, float radius);
        int objects_within_sphere(const ObjectType type, float x, float y, float z, float radius);
        int objects_within_sphere(const ObjectType* types, const int n_types, float x, float y, float z, float radius);
        int filter_active_objects(const ObjectType type);
        
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
