#pragma once

#define OBJECT_POOL_DEBUG 1

#ifdef OBJECT_POOL_DEBUG
    #define OBJECT_POOL_OBJECT_MACRO bool allocated;
#else
    #define OBJECT_POOL_OBJECT_MACRO
#endif

template <class Object, int BUFFER_POOL_SIZE>
class Object_pool {
    private:
        int batch_num;
        int instance_used;
        //batch malloc/link method
        void batch_alloc();

        virtual const char* name() { return "generic object pool"; }

    public:

    //virtual const char* name() = 0;
    //virtual const char* name() { static const char x[]= "generic object pool"; return x; }


    Object* first;
    Object* last;

    inline Object* acquire() __attribute__((always_inline));

    inline void retire(Object* nmb) __attribute__((always_inline));

    Object_pool();

};

template <class Object, int BUFFER_POOL_SIZE>
Object_pool<Object, BUFFER_POOL_SIZE>::Object_pool()
{
    batch_num = 0;
    first = NULL;
}

template <class Object, int BUFFER_POOL_SIZE>
void Object_pool<Object, BUFFER_POOL_SIZE>::batch_alloc()
{
    batch_num++;

    Object* ar = new Object[BUFFER_POOL_SIZE];

    #ifdef OBJECT_POOL_DEBUG
        for(int i=0; i<BUFFER_POOL_SIZE; i++) ar[i].allocated = false;
    #endif

    first = &ar[0];

    for(int i=0;i<BUFFER_POOL_SIZE-1; i++)
    {
        ar[i].next = &ar[i+1];
    }
    ar[BUFFER_POOL_SIZE-1].next = NULL;

    static const char* _name = name();
    printf("%s: Batch Alloc: %i n_elements: %i \n", _name, batch_num, BUFFER_POOL_SIZE);
}

template <class Object, int BUFFER_POOL_SIZE>
Object* Object_pool<Object, BUFFER_POOL_SIZE>::acquire()
{
    if(first == NULL) 
    {
        batch_alloc();       
    }
    Object* tmp = first;
    first = first->next;

    #ifdef OBJECT_POOL_DEBUG
        if(tmp->allocated != true)
        {
            static const char* _name = name();
            printf("%s: Error, attempted to acquire object that has not been returned to pool \n", _name);
            int segfault = *((int*) NULL);
        }
    #endif
    return tmp;
}

template <class Object, int BUFFER_POOL_SIZE>
void Object_pool<Object, BUFFER_POOL_SIZE>::retire(Object* nmb)
{
    #ifdef OBJECT_POOL_DEBUG
        if(nmb->allocated != false)
        {
            //static const char* _name = name();
            //printf("%s: Batch Alloc: %i n_elements: %i \n", _name, batch_num, BUFFER_POOL_SIZE);
        }
    #endif
    nmb->next = first;
    first = nmb;
}
