#pragma once 

#include <common/macros.hpp>

/*
 * Objects managed by MultiObject_list must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
 
//#include <common/common.hpp>
//#include <physics/common.hpp>

#define OBJECT_LIST_DEBUG 0

template <class Object_interface, int max_n=1024>
class MultiObject_list
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
        static const int n_max = max_n;
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
        MultiObject_list(Object_interface* (*create_interface)(int, int)); //default constructor
        virtual ~MultiObject_list(); //default deconstructor
};

template <class Object_interface, int max_n> 
MultiObject_list<Object_interface, max_n>::MultiObject_list(Object_interface* (*create_interface_fn)(int, int))
:
id_c(0),
num(0),
create_interface(create_interface_fn)
{
    this->a = (Object_interface**)calloc(max_n, sizeof(Object_interface*));
    //where();
}

template <class Object_interface, int max_n> 
MultiObject_list<Object_interface, max_n>::~MultiObject_list()
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

template <class Object_interface, int max_n>
void MultiObject_list<Object_interface, max_n>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_interface, int max_n>
Object_interface* MultiObject_list<Object_interface, max_n>::get(int id)
{
    if((id < 0) || (id >= n_max)) {
        return NULL;
    } 
    if(a[id] == NULL) {
        return NULL;
    }
    return a[id];
}

template <class Object_interface, int max_n>
void MultiObject_list<Object_interface, max_n>::print_members()
{
    printf("%s members:\n", name());
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        printf("%d\n", i);
    }
}

template <class Object_interface, int max_n>
int MultiObject_list<Object_interface, max_n>::get_free_id()
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

template <class Object_interface, int max_n>
Object_interface* MultiObject_list<Object_interface, max_n>::create(int type)
{
    GS_ASSERT(create_interface != NULL);
    if (create_interface == NULL) return NULL;
    //where();
    int i;
    int id;
    for(i=0; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if(i==n_max)
    {
        printf("%s_list Error: cannot create object, object limit exceeded\n", name());
        return NULL;
    }
    a[id] = this->create_interface(type, id);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_interface, int max_n>
Object_interface* MultiObject_list<Object_interface, max_n>::create(int type, int id)
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

template <class Object_interface, int max_n>
Object_interface* MultiObject_list<Object_interface, max_n>::get_or_create(int type, int id)
{
    //where();
    Object_interface* obj = a[id];
    if (obj == NULL) {
        obj = create(type, id);
    }
    return obj;
}

template <class Object_interface, int max_n>
bool MultiObject_list<Object_interface, max_n>::contains(int id)
{
    //where();
    Object_interface* obj = a[id];
    if (obj == NULL) {
        return false;
    }
    return true;
}

template <class Object_interface, int max_n>
void MultiObject_list<Object_interface, max_n>::destroy(int id)
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
 
template <class Object_interface, int max_n>
bool MultiObject_list<Object_interface, max_n>::full()
{
    GS_ASSERT(this->num <= max_n);
    return (this->num >= max_n);
}
