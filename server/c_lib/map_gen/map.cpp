#include "map.hpp"

#define V_SAMPLES 25*25

//using namespace noise::module;

//struct Point {
    //int x;
    //int y;
//};

//struct Point v_points[V_SAMPLES];

point v_points[V_SAMPLES];


/* Random methods */

inline int randx() {
    return rand() % xmax;
}
inline int randy() {
    return rand() % ymax;
}
inline int randz() {
    return rand() % zmax;
}

// returns random double in range [0,1]
inline double randd() {
    return ((double)rand()/(double)RAND_MAX);
}

inline double randdx() {
    return randd() * (double)xmax;
}
inline double randdy() {
    return randd() * (double)ymax;
}
inline double randdz() {
    return randd() * (double)zmax;
}


void set_seed(int seed) {
    srand((unsigned)seed);
    //Voroni::SetSeed(seed);
}

// fill v_points array with random integer points
//void init_v_points() {
    //int i;
    //struct Point* p;
    //for (i=0; i<V_SAMPLES; i++) {
        //p = &v_points[i];
        //if (p == NULL) {
            //p = (struct Point*) malloc(sizeof(struct Point));
            //v_points[i] = p;
        //}
        //p->x = randx();
        //p->y = randy();
    //}
//}
void init_v_points() {
    int i;
    point p;
    for (i=0; i<V_SAMPLES; i++) {
        p.x = randdx();
        p.y = randdy();
        v_points[i]=p;
    }
}

// create random points
// generate voroni polygons
// apply Lloyd -- move point to center of polygon
// do this N times, regenerating voroni each time
// see: http://en.wikipedia.org/wiki/Centroid

//double Voronoi::GetValue(x,y,z)  -- can i get the graph from this module?

void load_v_points() {
    load_points(v_points, V_SAMPLES);
}


void start_voronoi() {
    load_v_points();
    process_points();
}
