#pragma once 

//#include <common/common.hpp>
 
#define SIMPLE_OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024>
class Simple_object_list
{
    private:
        virtual const char* name() = 0;
        
    public:
        Object_state* a;

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
}

template <class Object_state, int max_n> 
Simple_object_list<Object_state, max_n>::~Simple_object_list()
{
    delete[] this->a;
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
    if(num >= max_n)
    {
        #if !PRODUCTION
        printf("%s list: max reached\n", this->name());
        #endif
        return NULL;
    }

    Object_state* obj = &a[num];
    num++;
    return obj;
}


template <class Object_state, int max_n>
void Simple_object_list<Object_state, max_n>::destroy(int index)
{
    GS_ASSERT(num > 0);
    GS_ASSERT(index >= 0 && index < max_n);
    if (index < 0 || index >= max_n) return;

    num--;

    // swap
    Object_state tmp = this->a[index];
    this->a[index] = this->a[num];
    this->a[num] = tmp;
}
