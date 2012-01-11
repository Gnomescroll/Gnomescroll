#pragma once 

/*
 * Objects managed by Object_list must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */

#define OBJECT_LIST_DEBUG 1

template <class Object_state, int max_n=1024>
class Object_list {
    private:
        int id_c;
        virtual const char* name() { return "Object"; }
    public:
        static const int n_max = max_n;
        int num;

    #if OBJECT_LIST_DEBUG
        Object_state** a;
    #else
        Object_state* a[max_n];
    #endif

        Object_list(); //default constructor
        ~Object_list(); //default deconstructor

        Object_state* get(int id);
        Object_state* create();         //object auto id
        Object_state* create(int id);   //create object with id
        Object_state* create(float x, float y, float z, float vx, float vy, float vz);
        Object_state* create(int id, float x, float y, float z, float vx, float vy, float vz);
        Object_state* get_or_create(int id);

        bool contains(int id);

        void destroy(int _id);

        void draw();    //overide in template specilization on client
        void draw(int all);
        void client_tick(); //override on template specilization
        void server_tick(); //override on template specilization

        void where();
};

//template <class T>
//Stack<T>::Stack(int s)

template <class Object_state, int max_n> 
Object_list<Object_state, max_n>::Object_list()
{
    #if OBJECT_LIST_DEBUG
        a = new Object_state* [max_n];
    #endif
    num = 0;
    id_c = 0;
    int i;
    for(i=0;i<max_n;i++) a[i] = NULL;
    printf("%s list instantiated at %p\n", this->name(), this);
    //where();
}


template <class Object_state, int max_n> 
Object_list<Object_state, max_n>::~Object_list()
{
    #if OBJECT_LIST_DEBUG
        delete[] a;
    #endif
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
        printf("%s id error: id=%i\n", name() ,id);
        return NULL;
    } 
    if(a[id] == NULL) {
        printf("%s get error: object is null, id=%i\n",name(), id);
        return NULL;
    }
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create() {
    //where();
    int i;
    int id = id_c;
    id_c++;
    for(i=0; i<n_max;i++) {
        id = (i+id)%n_max;
        if(a[id] == NULL) break;
    }
    if(i==1024) {
        printf("%s_list Error: cannot create object, object limit exceeded\n", name() );
        return NULL;
    }
    num++;
    a[id] = new Object_state(id);
    //printf("%s_list: Created object %i\n", name(), id);
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id) {
    //where();
    if(a[id] == NULL) {
        a[id] = new Object_state(id);
        //printf("%s_list: Created object from id: %i\n", name(), id);
        num++;
        return a[id];
    } else {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(float x, float y, float z, float vx, float vy, float vz) {
    int i;
    int id = id_c;
    id_c++;
    for(i=0; i<n_max;i++) {
        id = (i+id)%n_max;
        if(a[id] == NULL) break;
    }
    if(i==1024) {
        printf("%s_list Error: cannot create object, object limit exceeded\n", name() );
        return NULL;
    }
    num++;
    a[id] = new Object_state(id, x,y,z, vx,vy,vz);
    //printf("%s_list: Created object %i\n", name(), id);
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id, float x, float y, float z, float vx, float vy, float vz) {

    if(a[id] == NULL) {
        a[id] = new Object_state(id, x,y,z, vx,vy,vz);
        num++;
        //printf("%s_list: Created object from id: %i\n", name(), id);
        return a[id];
    } else {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
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
Object_state* Object_list<Object_state, max_n>::get_or_create(int id) {
    //where();
    Object_state* obj = a[id];
    if (obj == NULL) {
        obj = create(id);
    }
    return obj;
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::destroy(int id) {
    //where();
    if(a[id]==NULL) {
        printf("%s_list: Cannot delete object: object is null\n", name() );
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    //printf("%s_list: Deleted object %i\n",name(), id);
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::draw() {
    
    #ifdef DC_CLIENT
    // actual implementation will only draw ids from a global to_draw list
    int i;
    for (i=0; i<n_max;i++) {
        if (a[i]==NULL) continue;
        a[i]->draw();
    }
    #endif
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::draw(int all) {
    
    #ifdef DC_CLIENT
    if (!all) return;
    int i;
    for (i=0; i<n_max;i++) {
        if (a[i]==NULL) continue;
        a[i]->draw();
    }
    #endif
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::client_tick() {
    int i;
    for(i=0;i<n_max;i++){
        if(a[i]==NULL) continue;
        a[i]->client_tick();
    }   
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::server_tick() {
    int i;
    for(i=0;i<n_max;i++){
        if(a[i]==NULL) continue;
        a[i]->server_tick();
    }   
}
