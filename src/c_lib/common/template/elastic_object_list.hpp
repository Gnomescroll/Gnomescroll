#pragma once 

#include <new>

// WARNING: Any item tracked by this must 
 
template <class ObjectState, unsigned int minimum_capacity>
class ElasticObjectList
{
    private:
        virtual const char* name() = 0;
        
    public:

        ObjectState*  objects;        // the actual object array

        unsigned int hard_max;
        unsigned int n_max;
        unsigned int ct;
        unsigned int start;

        explicit ElasticObjectList(unsigned int hard_max);
        virtual ~ElasticObjectList();

        inline ObjectState* create();
        inline void destroy(int id);

        ObjectState* get(int id)
        {
            GS_ASSERT(id >= 0 && (unsigned int)id < this->n_max);
            if (id < 0 || (unsigned int)id >= this->n_max) return NULL;
            ObjectState* obj = &this->objects[id];
            if (obj->id == -1) return NULL;
            return obj;
        }

        inline void grow()
        {
            if (this->n_max >= this->hard_max) return;
            unsigned int new_max = this->n_max * 2;
            if (new_max > this->hard_max) new_max = this->hard_max;
            ObjectState* _objects = (ObjectState*)realloc(this->objects, new_max*sizeof(ObjectState));
            GS_ASSERT(_objects != NULL);
            if (_objects != NULL)
            {
                this->objects = _objects;
                for (unsigned int i=this->n_max; i<new_max; i++) this->objects[i].id = -1;
                this->n_max = new_max;
            }
        }

        inline void print();
};

template <class ObjectState, unsigned int minimum_capacity>
ElasticObjectList<ObjectState, minimum_capacity>::ElasticObjectList(unsigned int hard_max)
    : hard_max(hard_max), n_max(minimum_capacity), ct(0), start(0)
{
    GS_ASSERT(hard_max >= minimum_capacity);
    if (hard_max < minimum_capacity) this->hard_max = minimum_capacity;
    this->objects = (ObjectState*)malloc(this->n_max * sizeof(ObjectState));
    new (this->objects) ObjectState[this->n_max];
    for (unsigned int i=0; i<this->n_max; i++) this->objects[i].id = -1;
}

template <class ObjectState, unsigned int minimum_capacity>
ElasticObjectList<ObjectState, minimum_capacity>::~ElasticObjectList()
{
    if (this->objects != NULL)
    {
        for (unsigned int i=0; i<this->ct; i++)
            if (this->objects[i].id != -1)
                this->objects[i].ObjectState::~ObjectState();
        free(this->objects);
    }
}

template <class ObjectState, unsigned int minimum_capacity> 
inline void ElasticObjectList<ObjectState, minimum_capacity>::print()
{
    printf("%s list instantiated at %p\n", this->name(), this);
}

template <class ObjectState, unsigned int minimum_capacity>
inline ObjectState* ElasticObjectList<ObjectState, minimum_capacity>::create() 
{
    if (this->ct >= this->n_max) this->grow(); // expand
    if (this->ct >= this->n_max) return NULL;  // could not expand

    for (unsigned int i=0; i<this->n_max; i++)
    {
        int index = (i+this->start)%this->n_max;
        if (this->objects[index].id == -1)
        {
            this->ct++;
            this->start = index;
            new (&this->objects[index]) ObjectState;
            this->objects[index].id = index;
            return &this->objects[index];
        }
    }
    return NULL;
}

template <class ObjectState, unsigned int minimum_capacity>
inline void ElasticObjectList<ObjectState, minimum_capacity>::destroy(int id)
{
    GS_ASSERT(this->ct >= 0);
    GS_ASSERT(id >= 0 && (unsigned int)id < this->n_max);
    if (id < 0 || (unsigned int)id >= this->n_max) return;
    GS_ASSERT(this->objects[id].id != -1);
    if (this->objects[id].id == -1) return;
    this->objects[id].ObjectState::~ObjectState();
    this->objects[id].id = -1;
    this->ct--;
}
