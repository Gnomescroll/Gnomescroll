#pragma once 

//#include <common/common.hpp>
 
#define SIMPLE_OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024>
class Simple_object_list {
    private:
        virtual const char* name() = 0;
        
    public:
        Object_state* a;
        bool* used;

        static const int n_max = max_n;
        int num;
        int id_c;

        Simple_object_list(); //default constructor
        ~Simple_object_list(); //default deconstructor

        Object_state* create();         //object auto id
        
        void destroy(int _id);

        void print();
};

template <class Object_state, int max_n> 
Simple_object_list<Object_state, max_n>::Simple_object_list()
:
num(0),
id_c(0)
{
    this->a = new Object_state[max_n];
    for (int i=0; i<max_n; i++) this->a[i].id = i;
    this->used = (bool*)calloc(max_n, sizeof(bool));
    for (int i=0; i<max_n; i++) this->used[i] = false;
}

template <class Object_state, int max_n> 
Simple_object_list<Object_state, max_n>::~Simple_object_list()
{
    delete[] this->a;
    free(this->used);
}

template <class Object_state, int max_n> 
void Simple_object_list<Object_state, max_n>::print()
{
    const char* n = this->name();
    printf("%s list instantiated at %p\n", n, this);
}

template <class Object_state, int max_n>
Object_state* Simple_object_list<Object_state, max_n>::create() 
{
    GS_ASSERT(n_max == max_n);
    if(num >= max_n)
    {
        printf("%s list: max reached\n", this->name());
        return NULL;
    }

    int id;
    int i=0;
    for (; i<max_n; i++)
    {
        id = (i+id_c)%max_n;
        if (!this->used[id]) break;
    }

    GS_ASSERT(i < max_n);
    if (i >= max_n) return NULL;

    GS_ASSERT(!this->used[id]);

    this->used[id] = true;
    num++;
    id_c = id;
    return &a[id];
}


template <class Object_state, int max_n>
void Simple_object_list<Object_state, max_n>::destroy(int id)
{
    GS_ASSERT(id >= 0 && id < max_n);
    if (id < 0 || id >= max_n) return;

    GS_ASSERT(this->used[id]);

    num--;
    id_c = id;
    this->used[id] = false;
}
