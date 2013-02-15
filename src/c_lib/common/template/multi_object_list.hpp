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

    bool id_in_range(int id)
    {
        return (id >=0 && (size_t)id < this->n_max);
    }

    protected:
        int id_c;

    public:
        size_t n_max;
        int num;

        // pointer to function that accepts type,id and creates obj
        Object_interface* (*create_interface)(int, int);

        Object_interface** a;

    Object_interface* get(int id);
    Object_interface* create(int type);         //object auto id
    Object_interface* create(int type, int id);   //create object with id

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
    MultiObject_list(size_t capacity, Object_interface* (*create_interface)(int, int)); //default constructor
    virtual ~MultiObject_list(); //default deconstructor
};

template <class Object_interface>
MultiObject_list<Object_interface>::MultiObject_list(size_t capacity, Object_interface* (*create_interface_fn)(int, int)) :
    id_c(0), n_max(capacity), num(0), create_interface(create_interface_fn)
{
    this->a = (Object_interface**)calloc(this->n_max, sizeof(Object_interface*));
    //where();
}

template <class Object_interface>
MultiObject_list<Object_interface>::~MultiObject_list()
{
    if (a != NULL)
        for (size_t i=0; i<this->n_max; i++)
            delete this->a[i];
    free(this->a);
}

template <class Object_interface>
void MultiObject_list<Object_interface>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_interface>
Object_interface* MultiObject_list<Object_interface>::get(int id)
{
    if (!this->id_in_range(id)) return NULL;
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
    size_t i = 0;
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
    size_t i = 0;
    int id = 0;
    for (; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if (a[id] == NULL) break;
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
    IF_ASSERT(!this->id_in_range(id)) return NULL;
    if (a[id] == NULL)
    {
        a[id] = this->create_interface(type, id);
        num++;
        return a[id];
    }
    else
    {
        GS_ASSERT(false);
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_interface>
bool MultiObject_list<Object_interface>::contains(int id)
{
    IF_ASSERT(!this->id_in_range(id)) return false;
    return (a[id] != NULL);
}

template <class Object_interface>
void MultiObject_list<Object_interface>::destroy(int id)
{
    IF_ASSERT(!this->id_in_range(id)) return;
    IF_ASSERT(a[id] == NULL)
    {
        printf("%s_list: Cannot delete object %d: object is null\n", name(), id);
        return;
    }

    delete a[id];
    a[id] = NULL;
    num--;
}

template <class Object_interface>
bool MultiObject_list<Object_interface>::full()
{
    GS_ASSERT(this->num <= this->n_max);
    return (this->num >= this->n_max);
}
