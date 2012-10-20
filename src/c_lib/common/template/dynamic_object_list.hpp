#pragma once 

#include <common/macros.hpp>

#define OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024, int HARD_MAX=0>   // 0 means unlimited
class DynamicObjectList
{
    private:
        //#if PRODUCTION
        //const char* name() { return "Object"; }
        //#else
        virtual const char* name() = 0;
        //#endif

    protected:
        int id_c;
        
    public:
        static const int n_max_base = max_n;
        int n_max;
        int num;

        Object_state** a;

        Object_state* get(int id);
        Object_state* create();         //object auto id
        Object_state* create(int id);   //create object with id
        
        Object_state* get_or_create(int id);

        void resize(int new_size);

        bool contains(int id);
        bool full();

        int get_free_id();
        
        void destroy(int _id);

        void where();
        void print_members();

        void print()
        {
            printf("%s list instantiated at %p\n", this->name(), this);
        }

        DynamicObjectList(); //default constructor
        virtual ~DynamicObjectList(); //default deconstructor
};

template <class Object_state, int max_n, int HARD_MAX> 
DynamicObjectList<Object_state, max_n, HARD_MAX>::DynamicObjectList()
:
id_c(0),
n_max(max_n),
num(0)
{
    this->a = (Object_state**)calloc(max_n, sizeof(Object_state*));
    //where();
}

template <class Object_state, int max_n, int HARD_MAX> 
DynamicObjectList<Object_state, max_n, HARD_MAX>::~DynamicObjectList()
{
    if (a != NULL)
    {
        for (int i=0; i<this->n_max; i++)
        {
            if (this->a[i] != NULL)
                delete this->a[i];
        }
        free(this->a);
    }
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::get(int id)
{
    if (id < 0 || id >= n_max) return NULL;
    if (a[id] == NULL) return NULL;
    return a[id];
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::print_members()
{
    printf("%s members:\n", name());
    for (int i=0; i<n_max; i++)
        if (a[i] != NULL)
            printf("%d\n", i);
}

template <class Object_state, int max_n, int HARD_MAX>
int DynamicObjectList<Object_state, max_n, HARD_MAX>::get_free_id()
{
    int i;
    int id;
    for (i=0; i<n_max; i++)
    {
        id = (i + id_c) % n_max;
        if (a[id] == NULL) break;
    }
    if (i == n_max)
    {
        printf("%s_list Error: no free ids found\n", name());
        return -1;
    }
    return id;
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::resize(int new_size)
{
    if (HARD_MAX && new_size >= HARD_MAX) new_size = HARD_MAX;
    if (this->n_max == new_size) return;
    printf("Resizing %s list from %d to %d\n", this->name(), this->n_max, new_size);
    int old_size = this->n_max;
    this->a = (Object_state**)realloc(this->a, new_size * sizeof(Object_state**));
    GS_ASSERT(this->a != NULL);
    if (this->a == NULL) return;
    // initialize to NULL
    for (int i=old_size; i<new_size; this->a[i++] = NULL);
    this->n_max = new_size;
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::create()
{
    //where();
    GS_ASSERT(n_max > 0);
    if (n_max <= 0) return NULL;
    int i;
    int id=0;
    for (i=0; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if (a[id] == NULL) break;
    }
    if (i == n_max)
    {
        id = n_max; // save next id
        int new_size = this->n_max + n_max_base;
        this->resize(new_size);
    }
    a[id] = new Object_state(id);
    num++;
    id_c = id + 1;
    return a[id];
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::create(int id)
{
    if (id >= this->n_max)
    {   // need to resize
        int new_size = ((id/this->n_max_base) + 1) * this->n_max_base;
        this->resize(new_size);
    }
    GS_ASSERT(a[id] == NULL);
    if(a[id] == NULL) {
        a[id] = new Object_state(id);
        num++;
        return a[id];
    } else {
        GS_ASSERT(a[id] != NULL);
        return NULL;
    }
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::get_or_create(int id)
{
    Object_state* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) obj = create(id);
    return obj;
}

template <class Object_state, int max_n, int HARD_MAX>
bool DynamicObjectList<Object_state, max_n, HARD_MAX>::contains(int id)
{
    Object_state* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) return false;
    return true;
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::destroy(int id)
{
    GS_ASSERT(id < this->n_max);
    GS_ASSERT(this->a[id] != NULL);
    if (id >= this->n_max) return;
    if (this->a[id] == NULL) return;
    
    delete a[id];
    this->a[id] = NULL;
    this->num--;
}
 
template <class Object_state, int max_n, int HARD_MAX>
bool DynamicObjectList<Object_state, max_n, HARD_MAX>::full()
{
    GS_ASSERT(this->num <= max_n);
    return (this->num >= max_n);
}
