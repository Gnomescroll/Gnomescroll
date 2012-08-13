#pragma once 
 
#define SIMPLE_OBJECT_LIST_DEBUG 0

template <class Object_state, unsigned int max_n=1024>
class Simple_object_list
{
    private:
        virtual const char* name() = 0;
        
    public:
        Object_state a[max_n];

        static const int n_max = max_n;
        unsigned int num;
        unsigned int id_c;

        Simple_object_list(); //default constructor
        virtual ~Simple_object_list(); //default deconstructor

        inline Object_state* create();         //object auto id
        
        inline void destroy(int _id);

        inline void print();
};

template <class Object_state, unsigned int max_n> 
Simple_object_list<Object_state, max_n>::Simple_object_list()
:
num(0),
id_c(0)
{
    for (unsigned int i=0; i<max_n; i++) this->a[i].id = i;
}

template <class Object_state, unsigned int max_n> 
Simple_object_list<Object_state, max_n>::~Simple_object_list()
{
}

template <class Object_state, unsigned int max_n> 
inline void Simple_object_list<Object_state, max_n>::print()
{
    printf("%s list instantiated at %p\n", this->name(), this);
}

template <class Object_state, unsigned int max_n>
inline Object_state* Simple_object_list<Object_state, max_n>::create() 
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


template <class Object_state, unsigned int max_n>
inline void Simple_object_list<Object_state, max_n>::destroy(int index)
{
    GS_ASSERT(num > 0);
    unsigned int uindex = index;
    GS_ASSERT(index >= 0 && uindex < max_n);
    num--;
    this->a[uindex].reset();
    this->a[uindex] = this->a[num]; // swap last good object with dead object
    this->a[uindex].id = index; // update id of swapped object
}
