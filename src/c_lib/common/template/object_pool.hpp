/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

//keeps track of alloc and dealloc count for objects
#define ENTITY_POOL_DEBUG 0

//enabling this will turn off object pools and use malloc and free
#define ENTITY_POOL_DEBUG_BATCH 0

#if ENTITY_POOL_DEBUG
    #define ENTITY_POOL_ENTITY_MACRO int allocated;
#else
    #define ENTITY_POOL_ENTITY_MACRO
#endif

template <class Base, class Entity, int BUFFER_POOL_SIZE>
class ObjectPool
{
    private:
        int batch_num;
        int instance_used;
        //batch malloc/link method
        void batch_alloc();

        //garabage collection
        Entity** alloc_list;
        int alloc_list_index;
        int alloc_list_max_size;
        //virtual char* name() { static char* x = (char*) "Error: generic object pool"; return x; }

    public:

        Entity* first;
        Entity* last;

    inline Entity* acquire() __attribute__((always_inline));

    inline void retire(Entity* nmb) __attribute__((always_inline));

    ObjectPool();
    ~ObjectPool();
};

template <class Base, class Entity, int BUFFER_POOL_SIZE>
ObjectPool<Base, Entity, BUFFER_POOL_SIZE>::ObjectPool()
{
    //batch_num = 0;
    //first = NULL;
}

template <class Base, class Entity, int BUFFER_POOL_SIZE>
ObjectPool<Base, Entity, BUFFER_POOL_SIZE>::~ObjectPool()
{
    /*
        Warning: batch_alloc() must be called at least once
    */
    for (int i=0; i<alloc_list_index; i++) delete[] alloc_list[i];
    delete[] alloc_list;
}


template <class Base, class Entity, int BUFFER_POOL_SIZE>
void ObjectPool<Base, Entity, BUFFER_POOL_SIZE>::batch_alloc()
{
    static int inited = 0;
    if (inited == 0)
    {
        inited = 1;

        batch_num = 0;
        first = NULL;

        const int INITIAL_SIZE = 4;
        alloc_list = new Entity*[INITIAL_SIZE];
        alloc_list_max_size = INITIAL_SIZE;
        alloc_list_index = 0;
    }

    batch_num++;

    Entity* ar = new Entity[BUFFER_POOL_SIZE];

    if (ar == NULL)
    {
        printf("%s: Batch Alloc  %i:NEW RETURNED NULL, MEMORY ERROR! \n", Base::name(), batch_num);
    }

    #if ENTITY_POOL_DEBUG
        for (int i=0; i<BUFFER_POOL_SIZE; i++) ar[i].allocated = 0;
    #endif

    first = &ar[0];

    for (int i=0;i<BUFFER_POOL_SIZE-1; i++)
    {
        ar[i].next = &ar[i+1];
    }
    ar[BUFFER_POOL_SIZE-1].next = NULL;

    //static char* _name = name();
    //printf("%s: Batch Alloc: %i n_elements: %i \n", Base::name(), batch_num, BUFFER_POOL_SIZE);

    alloc_list[alloc_list_index] = ar;
    alloc_list_index++;

    if (alloc_list_index >= alloc_list_max_size)
    {
        if (alloc_list_index > alloc_list_max_size) printf("%s: Batch Alloc: ERROR!! alloc_list error!\n", Base::name());
        printf("%s: Batch Alloc: possible memory leak!\n", Base::name());
        Entity** tmp = new Entity*[2*alloc_list_max_size];
        for (int i=0; i < alloc_list_max_size; i++) tmp[i] = alloc_list[i];
        delete[] alloc_list;
        alloc_list = tmp;
        alloc_list_max_size *= 2;
    }

}

template <class Base, class Entity, int BUFFER_POOL_SIZE>
ALWAYS_INLINE Entity* ObjectPool<Base, Entity, BUFFER_POOL_SIZE>::acquire()
{
    #if ENTITY_POOL_DEBUG_BATCH
        Entity* tmp2 = (Entity*) malloc(sizeof(Entity));
        tmp2->next = NULL; //debug
        return tmp2;
    #endif

    if (first == NULL)
    {
        batch_alloc();
    }
    Entity* tmp = first;
    first = first->next;

    #if ENTITY_POOL_DEBUG
        if (tmp->allocated != 0)
        {
            //static const char* _name = name();
            printf("%s: Memory Pool Acquire Error, allocated= %i, object= %lx \n", Base::name(), tmp->allocated, (long) tmp);
            //int segfault = *((int*) NULL);
        }
        tmp->allocated++;
    #endif
    return tmp;
}

template <class Base, class Entity, int BUFFER_POOL_SIZE>
ALWAYS_INLINE void ObjectPool<Base, Entity, BUFFER_POOL_SIZE>::retire(Entity* nmb)
{
    #if ENTITY_POOL_DEBUG_BATCH
        free(nmb);
        return;
    #endif

    #if ENTITY_POOL_DEBUG
        if (nmb->allocated != 1)
        {
            //static const char* _name = name();
            printf("%s: Memory Pool Retire Error, allocated= %i, object= %lx \n", Base::name(), nmb->allocated, (long) nmb);
            int segfault = *((int*) NULL);
            printf("segfault= %i", segfault);
        }
        nmb->allocated--;
    #endif
    nmb->next = first;
    first = nmb;
}
