#pragma once 

#include <common/macros.hpp>

/*
 * Objects managed by DynamicMultiObjectList must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
 
//#include <common/common.hpp>
//#include <physics/common.hpp>

#define OBJECT_LIST_DEBUG 0

template <class Object_interface, int max_n=1024>
class DynamicMultiObjectList
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

        // pointer to function that accepts type,id and creates obj
        Object_interface* (*create_interface)(int, int);

        Object_interface** a;

        Object_interface* get(int id);
        Object_interface* create(int type);         //object auto id
        Object_interface* create(int type, int id);   //create object with id
        
        Object_interface* get_or_create(int type, int id);

        void resize(int new_size);

        bool contains(int id);
        bool full();

        int get_free_id();
        
        void destroy(int _id);

        void where();
        void print_members();

        // initialize with pointer to creator
        DynamicMultiObjectList(Object_interface* (*create_interface)(int, int)); //default constructor
        virtual ~DynamicMultiObjectList(); //default deconstructor
};

template <class Object_interface, int max_n> 
DynamicMultiObjectList<Object_interface, max_n>::DynamicMultiObjectList(Object_interface* (*create_interface)(int, int))
:
id_c(0),
n_max(max_n),
num(0),
create_interface(create_interface)
{
    this->a = (Object_interface**)calloc(max_n, sizeof(Object_interface*));
    //where();
}

template <class Object_interface, int max_n> 
DynamicMultiObjectList<Object_interface, max_n>::~DynamicMultiObjectList()
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
void DynamicMultiObjectList<Object_interface, max_n>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_interface, int max_n>
Object_interface* DynamicMultiObjectList<Object_interface, max_n>::get(int id)
{
    //where();
    if((id < 0) || (id >= n_max)) {
        //printf("%s id error: id=%i\n", name() ,id);
        //if (id != NO_AGENT) // TODO 
        //    print_trace();
        return NULL;
    } 
    if(a[id] == NULL) {
        //printf("%s get error: object is null, id=%i\n",name(), id);
        //this->print_members();
        return NULL;
    }
    return a[id];
}

template <class Object_interface, int max_n>
void DynamicMultiObjectList<Object_interface, max_n>::print_members()
{
    printf("%s members:\n", name());
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        printf("%d\n", i);
    }
}

template <class Object_interface, int max_n>
int DynamicMultiObjectList<Object_interface, max_n>::get_free_id()
{
    GS_ASSERT(n_max > 0);
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
void DynamicMultiObjectList<Object_interface, max_n>::resize(int new_size)
{
    printf("Resizing %s list from %d to %d\n", this->name(), this->n_max, new_size);
    GS_ASSERT(this->n_max < new_size);
    int old_size = this->n_max;
    this->a = (Object_interface**)realloc(this->a, new_size * sizeof(Object_interface**));
    GS_ASSERT(this->a != NULL);
    // initialize to NULL
    for (int i=old_size; i<new_size; this->a[i++] = NULL);
    this->n_max = new_size;
}

template <class Object_interface, int max_n>
Object_interface* DynamicMultiObjectList<Object_interface, max_n>::create(int type)
{
    GS_ASSERT(create_interface != NULL);
    GS_ASSERT(n_max > 0);
    //where();
    int i;
    int id=0;
    for(i=0; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if (i==n_max)
    {
        id = n_max; // save next id
        int new_size = this->n_max + n_max_base;
        this->resize(new_size);
    }
    a[id] = this->create_interface(type, id);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_interface, int max_n>
Object_interface* DynamicMultiObjectList<Object_interface, max_n>::create(int type, int id)
{
    if (id >= this->n_max)
    {   // need to resize
        int new_size = ((id/this->n_max_base) + 1) * this->n_max_base;
        this->resize(new_size);
    }
    GS_ASSERT(create_interface != NULL);
    //where();
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
Object_interface* DynamicMultiObjectList<Object_interface, max_n>::get_or_create(int type, int id)
{
    //where();
    Object_interface* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) {
        obj = create(type, id);
    }
    return obj;
}

template <class Object_interface, int max_n>
bool DynamicMultiObjectList<Object_interface, max_n>::contains(int id)
{
    //where();
    Object_interface* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) {
        return false;
    }
    return true;
}

template <class Object_interface, int max_n>
void DynamicMultiObjectList<Object_interface, max_n>::destroy(int id)
{
    //where();
    if(id >= this->n_max || a[id]==NULL) {
        printf("%s_list: Cannot delete object: object is null\n", name() );
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    //printf("%s_list: Deleted object %i\n",name(), id);
}
 
template <class Object_interface, int max_n>
bool DynamicMultiObjectList<Object_interface, max_n>::full()
{
    GS_ASSERT(this->num <= max_n);
    return (this->num >= max_n);
}
