#include "map.h"

#define V_SAMPLES 25*25

using namespace noise::module;

struct Point {
    int x;
    int y;
};

struct Point v_points[V_SAMPLES];



inline int randx() {
    return rand() % xmax;
}
inline int randy() {
    return rand() % ymax;
}
inline int randz() {
    return rand() % zmax;
}


void set_seed(unsigned int seed) {
    srand(seed);
    Voroni::SetSeed((int)seed);
}

// fill v_points array with random integer points
void init_v_points() {
    int i;
    struct Point* p;
    for (i=0; i<V_SAMPLES; i++) {
        p = &v_points[i];
        if (p == NULL) {
            p = (struct Point*) malloc(sizeof(struct Point));
            v_points[i] = p;
        }
        p->x = randx();
        p->y = randy();
    }
}

// create random points
// generate voroni polygons
// apply Lloyd -- move point to center of polygon
// do this N times, regenerating voroni each time
// see: http://en.wikipedia.org/wiki/Centroid
