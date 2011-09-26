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
    double d = ((double)rand()/(double)RAND_MAX);
    return d;
}

inline double randdx() {
    double d = randd() * xmax;
    return d;
}
inline double randdy() {
    double d = randd() * ymax;
    return d;
}
inline double randdz() {
    double d = randd() * zmax;
    return d;
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
        //printf("%Lf %Lf\n", p.x, p.y);
    }
}

// create random points
// generate voroni polygons
// apply Lloyd -- move point to center of polygon
// do this N times, regenerating voroni each time
// see: http://en.wikipedia.org/wiki/Centroid

//double Voronoi::GetValue(x,y,z)  -- can i get the graph from this module?

// need to be able to extract region given a point

// then can apply Lloyd's and do all the map stuff

void load_v_points() {
    load_points(v_points, V_SAMPLES);
}


void start_voronoi() {
    init_v_points();
    load_v_points();
    process_points();
}
