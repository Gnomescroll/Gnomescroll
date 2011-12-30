#pragma once

#include <c_lib/template/object_list.hpp>

namespace Dragon {

class Building {
    public:
        //int id;
        int x,y,z;
        int w,h,l;
        int tile;

        void set() {
            int i,j,k;
            for (i=x; i<w+x; i++) {
                for (j=y; j<h+y; j++) {
                    for (k=z; k<z+l; k++) {
                        if (   i > x && i < w+x-1
                            && j > y && j < h+y-1
                            && k != z+l-1
                        ) continue; // hollow
                        if ( k != z+l-1 && (k%3) ==2 && ((i > x && i < w+x-1) || (j > y && j < h+y-1)) && !randrange(0,2)) continue; // windows
                        _set(i,j,k,tile);
                    }
                }
            }
        }

        Building(){}
        Building(int id){}
        Building(int x, int y, int z, int w, int h, int l, int tile) :
        x(x), y(y), z(z),
        w(w), h(h), l(l),
        tile(tile)
        {}
};

static const int BUILDING_MAX = 64;
class Building_list: public Object_list<Building,BUILDING_MAX>
{
    private:
        const char* name() { return "Building"; }

    public:
        void set() {
            int i;
            for (i=0; i<BUILDING_MAX; i++) {
                if (a[i] == NULL) continue;
                a[i]->set();
            }
        }

        Building* add(int x, int y, int z, int w, int h, int l, int tile) {
            Building* b = this->create();
            b->x = x;
            b->y = y;
            b->z = z;
            b->w = w;
            b->h = h;
            b->l = l;
            b->tile = tile;
            return b;
        }
};

void l_system();
void generate();

//cython
void generate_dragon();

}
