#pragma once 

#include <new>
 
template <class ObjectState, unsigned int minimum_capacity>
class ElasticObjectList
{
    private:
        virtual const char* name() = 0;
        
    public:
        ObjectState* a;

        unsigned int n_max;
        unsigned int num;
        unsigned int start;

        ElasticObjectList(); //default constructor
        virtual ~ElasticObjectList(); //default deconstructor

        inline ObjectState* create();         //object auto id
        
        inline void destroy(int _id);

        inline void grow()
        {
            this->n_max *= 2;
            ObjectState* _a = (ObjectState*)realloc(this->a, this->n_max);
            GS_ASSERT(_a != NULL);
            if (_a != NULL) this->a = _a;
        }

        inline void shrink()
        {
            if (this->n_max / 2 <= minimum_capacity) return;  // we would shrink 
            static const float shrink_ratio = 0.8f; // when at 80% capacity of lower bound, shrink
            unsigned int limit = (unsigned int)((float)(this->n_max / 2) * shrink_ratio);
            if (this->num >= limit) return;

            int shrunk_size = this->n_max / 2;
            ObjectState* _a = (ObjectState*)realloc(this->a, shrunk_size);
            GS_ASSERT(_a != NULL);
            if (_a != NULL)
            {
                this->a = _a;
                new (&this->a[this->num]) ObjectState[shrunk_size - this->num];
                this->n_max = shrunk_size;
            }
        }

        inline void print();
};

template <class ObjectState, unsigned int minimum_capacity> 
ElasticObjectList<ObjectState, minimum_capacity>::ElasticObjectList() :
    n_max(minimum_capacity), num(0), start(0)
{
    this->a = (ObjectState*)malloc(this->n_max * sizeof(ObjectState));
    new (this->a) ObjectState[this->n_max];
    for (unsigned int i=0; i<this->n_max; i++) this->a[i].id = -1;
}

template <class ObjectState, unsigned int minimum_capacity> 
ElasticObjectList<ObjectState, minimum_capacity>::~ElasticObjectList()
{
    for (unsigned int i=0; i<this->num; i++)
        this->a[i].ObjectState::~ObjectState();
    free(this->a);
}

template <class ObjectState, unsigned int minimum_capacity> 
inline void ElasticObjectList<ObjectState, minimum_capacity>::print()
{
    printf("%s list instantiated at %p\n", this->name(), this);
}

template <class ObjectState, unsigned int minimum_capacity>
inline ObjectState* ElasticObjectList<ObjectState, minimum_capacity>::create() 
{
    if (this->num >= this->n_max)
        this->grow();

    for (unsigned int i=0; i<this->n_max; i++)
    {
        int index = (i+this->start) % this->n_max;
        if (this->a[index].id == -1)
        {
            this->num++;
            this->a[index].id = index;
            this->start = (index + 1) % this->n_max;
            return &this->a[index];
        }
    }

    return NULL;
}


template <class ObjectState, unsigned int minimum_capacity>
inline void ElasticObjectList<ObjectState, minimum_capacity>::destroy(int index)
{
    GS_ASSERT(index >= 0 && (unsigned int)index < this->n_max);
    if (index < 0 || (unsigned int)index >= this->n_max) return;
    GS_ASSERT(this->a[index].id != -1);
    this->a[num].reset();   // reset dead object    
    this->a[index].id = -1;

    this->shrink();
}
