#pragma once 

/*
 * Objects managed by Object_list must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
 
//#include <common/common.hpp>
//#include <physics/common.hpp>

#define OBJECT_LIST_DEBUG 0

void print_list(char* name, void* ptr)
{
    printf("%s list instantiated at %p\n", name, ptr);
}

template <class Object_state, int max_n=1024>
class Object_list {
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

        Object_state** a;

        Object_state* get(int id);
        Object_state* create();         //object auto id
        Object_state* create(int id);   //create object with id
        
        Object_state* get_or_create(int id);

        bool contains(int id);
        bool full();

        int get_free_id();
        
        void destroy(int _id);

        void where();
        void print_members();

        Object_list(); //default constructor
        virtual ~Object_list(); //default deconstructor
};

template <class Object_state, int max_n> 
Object_list<Object_state, max_n>::Object_list()
:
id_c(0),
num(0)
{
    this->a = (Object_state**)calloc(max_n, sizeof(Object_state*));
    //where();
}

template <class Object_state, int max_n> 
Object_list<Object_state, max_n>::~Object_list()
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

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::get(int id)
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

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::print_members() {
    int i;
    printf("%s members:\n", name());
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        printf("%d\n", i);
    }
}

template <class Object_state, int max_n>
int Object_list<Object_state, max_n>::get_free_id()
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

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create()
{
    //where();
    int i;
    int id;
    for(i=0; i<n_max;i++) {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if(i==n_max)
    {
        printf("%s_list Error: cannot create object, object limit %d exceeded\n", name(), this->n_max);
        return NULL;
    }
    a[id] = new Object_state(id);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id) {
    //where();
    GS_ASSERT(a[id] == NULL);
    if(a[id] == NULL)
    {
        a[id] = new Object_state(id);
        num++;
        return a[id];
    } else
    {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::get_or_create(int id) {
    //where();
    Object_state* obj = a[id];
    if (obj == NULL) {
        obj = create(id);
    }
    return obj;
}

template <class Object_state, int max_n>
bool Object_list<Object_state, max_n>::contains(int id) {
    //where();
    Object_state* obj = a[id];
    if (obj == NULL) {
        return false;
    }
    return true;
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::destroy(int id)
{
    //where();
    if(a[id]==NULL)
    {
        printf("%s_list: Cannot delete object %d: object is null\n", name(), id);
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
}
 
template <class Object_state, int max_n>
bool Object_list<Object_state, max_n>::full()
{
    GS_ASSERT(this->num <= max_n);
    return (this->num >= max_n);
}
