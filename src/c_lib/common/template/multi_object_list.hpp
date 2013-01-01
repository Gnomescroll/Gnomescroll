#pragma once 

#include <common/macros.hpp>

/*
 * Entities managed by MultiObject_list must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
 
//#include <common/common.hpp>
//#include <physics/common.hpp>

#define OBJECT_LIST_DEBUG 0

template <class Object_interface>
class MultiObject_list
{
    private:
        virtual const char* name() = 0;

    protected:
        int id_c;
        
    public:
        unsigned int n_max;
        int num;

        // pointer to function that accepts type,id and creates obj
        Object_interface* (*create_interface)(int, int);

        Object_interface** a;

        Object_interface* get(int id);
        Object_interface* create(int type);         //object auto id
        Object_interface* create(int type, int id);   //create object with id
        
        Object_interface* get_or_create(int type, int id);

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

        // initialize with pointer to creator
        MultiObject_list(unsigned int capacity, Object_interface* (*create_interface)(int, int)); //default constructor
        virtual ~MultiObject_list(); //default deconstructor
};

template <class Object_interface> 
MultiObject_list<Object_interface>::MultiObject_list(unsigned int capacity, Object_interface* (*create_interface_fn)(int, int)) :
    id_c(0), n_max(capacity), num(0), create_interface(create_interface_fn)
{
    this->a = (Object_interface**)calloc(this->n_max, sizeof(Object_interface*));
    //where();
}

template <class Object_interface> 
MultiObject_list<Object_interface>::~MultiObject_list()
{
    if (a != NULL)
    {
        for (unsigned int i=0; i<this->n_max; i++)
        {
            if (this->a[i] != NULL)
                delete this->a[i];
        }
        free(this->a);
    }
}

template <class Object_interface>
void MultiObject_list<Object_interface>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_interface>
Object_interface* MultiObject_list<Object_interface>::get(int id)
{
    if (id < 0 || (unsigned int)id >= n_max) return NULL;
    if (a[id] == NULL) return NULL;
    return a[id];
}

template <class Object_interface>
void MultiObject_list<Object_interface>::print_members()
{
    printf("%s members:\n", name());
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        printf("%d\n", i);
    }
}

template <class Object_interface>
int MultiObject_list<Object_interface>::get_free_id()
{
    unsigned int i = 0;
    int id = 0;
    for (; i<n_max; i++)
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

template <class Object_interface>
Object_interface* MultiObject_list<Object_interface>::create(int type)
{
    IF_ASSERT(create_interface == NULL) return NULL;
    //where();

    unsigned int i = 0;
    int id = 0;
    for (; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }

    IF_ASSERT(i == n_max)
    {
        printf("%s_list Error: cannot create object, object limit exceeded\n", name());
        return NULL;
    }
    
    a[id] = this->create_interface(type, id);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_interface>
Object_interface* MultiObject_list<Object_interface>::create(int type, int id)
{
    GS_ASSERT(create_interface != NULL);
    if (create_interface == NULL) return NULL;
    //where();
    GS_ASSERT(a[id] == NULL);
    if(a[id] == NULL)
    {
        a[id] = this->create_interface(type, id);
        num++;
        return a[id];
    }
    else
    {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_interface>
Object_interface* MultiObject_list<Object_interface>::get_or_create(int type, int id)
{
    //where();
    Object_interface* obj = a[id];
    if (obj == NULL) {
        obj = create(type, id);
    }
    return obj;
}

template <class Object_interface>
bool MultiObject_list<Object_interface>::contains(int id)
{
    //where();
    Object_interface* obj = a[id];
    if (obj == NULL) {
        return false;
    }
    return true;
}

template <class Object_interface>
void MultiObject_list<Object_interface>::destroy(int id)
{
    //where();
    if(a[id]==NULL) {
        printf("%s_list: Cannot delete object %d: object is null\n", name(), id);
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    //printf("%s_list: Deleted object %i\n",name(), id);
}
 
template <class Object_interface>
bool MultiObject_list<Object_interface>::full()
{
    GS_ASSERT(this->num <= this->n_max);
    return (this->num >= this->n_max);
}
