#pragma once 

/*
 * Objects managed by Object_list must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
#include <c_lib/physics/common.hpp>
 
#define OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024>
class Object_list {
    private:
        int id_c;
        virtual const char* name() = 0;

    protected:
        // quicksort helpers
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);
        void swap_object_state(Object_state **a, Object_state **b);
        void swap_float(float *a, float *b);
        
    public:
        static const int n_max = max_n;
        int num;

        Object_state** a;

        Object_list(); //default constructor
        ~Object_list(); //default deconstructor

        Object_state* get(int id);
        Object_state* create();         //object auto id

        Object_state* create(int id);   //create object with id
        Object_state* create(float x, float y, float z);
        Object_state* create(int id, float x, float y, float z);
        Object_state* create(float x, float y, float z, float vx, float vy, float vz);
        Object_state* create(int id, float x, float y, float z, float vx, float vy, float vz);
        Object_state* get_or_create(int id);

        bool contains(int id);
        bool full();
        
        void destroy(int _id);

        void draw();    //overide in template specilization on client
        void draw(int all);

        void where();
        void print();
        void print_members();

        // filtering
        Object_state* filtered_objects[max_n]; // tmp array for filtering objects
        float filtered_object_distances[max_n];
        int n_filtered;

        void filter_none(); // copies pointers/null into filtered list, unchanged
        int objects_within_sphere(float x, float y, float z, float radius);
        void objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold
        void sort_filtered_objects_by_distance(bool ascending=true);
};

//template <class T>
//Stack<T>::Stack(int s)

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
void Object_list<Object_state, max_n>::print()
{
    const char* n = this->name();
    printf("%s list instantiated at %p\n", n, this);
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
        printf("%s id error: id=%i\n", name() ,id);
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
Object_state* Object_list<Object_state, max_n>::create() {
    //where();
    int i;
    int id;
    for(i=0; i<n_max;i++) {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if(i==n_max) {
        printf("%s_list Error: cannot create object, object limit exceeded\n", name() );
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
    if(a[id] == NULL) {
        a[id] = new Object_state(id);
        num++;
        return a[id];
    } else {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(float x, float y, float z) {
    int i;
    int id;
    for(i=0; i<n_max;i++) {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if (i == n_max) return NULL;    // full
    a[id] = new Object_state(id, x,y,z);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id, float x, float y, float z) {
    if (a[id] == NULL)
    {
        a[id] = new Object_state(id, x,y,z);
        num++;
        return a[id];
    }
    else
    {
        printf("%s_list: Cannot create object from id. id %d is in use\n", name(), id);
        return NULL;
    }
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(float x, float y, float z, float vx, float vy, float vz) {
    int i;
    int id;
    for(i=0; i<n_max;i++) {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if (i == n_max) return NULL;    // full
    a[id] = new Object_state(id, x,y,z, vx,vy,vz);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id, float x, float y, float z, float vx, float vy, float vz) {

    if(a[id] == NULL) {
        a[id] = new Object_state(id, x,y,z, vx,vy,vz);
        num++;
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
void Object_list<Object_state, max_n>::destroy(int id)
{
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
bool Object_list<Object_state, max_n>::full()
{
    if (this->num > max_n) printf("Num %d exceed max_n %d !!\n", num, max_n);
    return (this->num >= max_n);
}


/* quicksorts */

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::swap_object_state(Object_state **a, Object_state **b)
{Object_state* t=*a; *a=*b; *b=t;}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::swap_float(float *a, float *b)
{float t=*a; *a=*b; *b=t;}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::quicksort_distance_asc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] <= dist)
                l++;
            else {
                swap_float(&this->filtered_object_distances[l], &this->filtered_object_distances[--r]);
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[r]);
            }
        }
        swap_float(&this->filtered_object_distances[--l], &this->filtered_object_distances[beg]);
        swap_object_state(&this->filtered_objects[l], &this->filtered_objects[beg]);
        quicksort_distance_asc(beg, l);
        quicksort_distance_asc(r, end);
    }
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::quicksort_distance_desc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] >= dist)
                l++;
            else {
                swap_float(&this->filtered_object_distances[l], &this->filtered_object_distances[--r]);
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[r]);
            }
        }
        swap_float(&this->filtered_object_distances[--l], &this->filtered_object_distances[beg]);
        swap_object_state(&this->filtered_objects[l], &this->filtered_objects[beg]);
        quicksort_distance_desc(beg, l);
        quicksort_distance_desc(r, end);
    }
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::sort_filtered_objects_by_distance(bool ascending)
{
    if (ascending) this->quicksort_distance_asc(0, this->n_filtered);
    else this->quicksort_distance_desc(0, this->n_filtered);
}

template <class Object_state, int max_n>
int Object_list<Object_state, max_n>::objects_within_sphere(
    float x, float y, float z, float radius
)
{
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    int i;
    for (i=0; i<max_n; i++) {
        if (a[i] == NULL) continue;
        dist = distancef(x,y,z, a[i]->x, a[i]->y, a[i]->z);
        if (dist < radius) {
            // agent in sphere
            filtered_objects[ct] = a[i];
            filtered_object_distances[ct] = dist;
            if (dist < min_dist) {
                min_dist = dist;
                closest = ct;
            }
            ct++;            
        }
    }
    this->n_filtered = ct;
    return closest;
}

// origin, direction, cone threshold
template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::objects_in_cone(
    float x, float y, float z, float vx, float vy, float vz, float theta
)
{
    int ct = 0;
    float ax,ay,az;
    float ip;
    float arc;

    float len = sqrt(vx*vx + vy*vy + vz*vz);
    vx /= len;
    vy /= len;
    vz /= len;
    for (int i=0; i<max_n; i++)
    {
        Object_state* a = this->a[i];
        if (a == NULL) continue;

        ax = a->x - x;
        ay = a->y - y;
        az = a->z - z;

        len = sqrt(ax*ax + ay*ay + az*az);
        ax /= len;
        ay /= len;
        az /= len;

        ip = ax*vx + ay*vy + az*vz;
        arc = abs(acos(ip));

        if (arc < theta)
            filtered_objects[ct++] = a;
    }

    this->n_filtered = ct;
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::filter_none()
{   // moves all non null objects to the filtered list
    int c = 0;
    for (int i=0; i<max_n; i++)
    {
        if (this->a[i] == NULL) continue;
        this->filtered_objects[c++] = this->a[i];
    }
    this->n_filtered = c;
}
