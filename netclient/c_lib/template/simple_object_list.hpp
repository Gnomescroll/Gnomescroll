#pragma once 

//#include <c_lib/common/common.hpp>
 
#define SIMPLE_OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024>
class Simple_object_list {
    private:
        virtual const char* name() = 0;
        
    public:
        Object_state* a;

        static const int n_max = max_n;
        int num;

        Simple_object_list(); //default constructor
        ~Simple_object_list(); //default deconstructor

        Object_state* create();         //object auto id
        
        void destroy(int _id);

        void print();
};

template <class Object_state, int max_n> 
Simple_object_list<Object_state, max_n>::Simple_object_list()
:
num(0)
{
    this->a = (Object_state*)calloc(max_n, sizeof(Object_state));
}

template <class Object_state, int max_n> 
Simple_object_list<Object_state, max_n>::~Simple_object_list()
{
    free(this->a);
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
        printf("client particle list: max reached\n");
        return NULL;
    }

    Object_state* o = &a[num];
    a[num].id = num;
    num++;
    return o;
    //return &a[num++];
}


template <class Object_state, int max_n>
void Simple_object_list<Object_state, max_n>::destroy(int id)
{
    if(id >= num)
    {
        printf("simple object list index error: num= %i, id= %i \n", num, id);
        return;
    }

    num--;
    a[id] = a[num];
    a[id].id = id;

}
