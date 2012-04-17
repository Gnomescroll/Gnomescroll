#pragma once

namespace Objects
{

/* Management */

const int MAX_OBJECT_TYPES = 256;
class ObjectList
{
    private:
        // keep a counters for individual object indices
        int* indices;
        int* maximums;
        Object** objects;
        
    public:
        int get_free_id(ObjectType type)
        {
            if (this->objects[type] == NULL) return -1;
            for (int i=0; i<this->maximums[type]; i++)
                if (this->objects[type][i] != NULL) return i;
            return -1;
        }
        
        void destroy(int id)
        {
            if (this->objects[type] == NULL) return;
            if (this->objects[type][id] == NULL) return;
            delete this->objects[type][id];
            this->objects[type][id] = NULL;
        }
        
        void create(ObjectType type, int id)
        {
            if (this->objects[type] == NULL) return NULL;
            if (this->objects[type][id] != NULL)
            {
                printf("WARNING: object %d,%d in use\n", type, id);
                return NULL;
            }
            this->objects[type][id] = new Object(id);
        }

        void set_object_max(ObjectType type, int max)
        {
            this->maximums[type] = max;
            this->objects[type] = (Object**)calloc(max, sizeof(Object*));
        }
        
        void init()
        {
            this->indices = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
            this->maximums = (int*)calloc(MAX_OBJECT_TYPES, sizeof(int));
            this->objects = (Object***)calloc(MAX_OBJECT_TYPES, sizeof(Object**));
        }

    ObjectList()
    : indices(NULL), maximums(NULL), objects(NULL)
    {}

    ~ObjectList()
    {
        if (this->objects != NULL)
        {
            for (int i=0; i<this->MAX_OBJECT_TYPES; i++)
                if (this->objects[i] != NULL)
                    for (int j=0; j<this->maximums[i]; j++)
                        delete this->objects[i][j];
                        
            free(this->objects);
        }
        
        if (this->indices != NULL) free(this->indices);
        if (this->maximums != NULL) free(this->maximums);
    }
};

} // Objects
