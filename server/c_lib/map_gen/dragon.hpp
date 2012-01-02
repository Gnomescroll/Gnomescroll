#pragma once

#include <c_lib/template/object_list.hpp>

#include <c_lib/common/random.h>

namespace Dragon {

class Building {
    public:
        //int id;
        int x,y,z;
        int w,h,l;
        int tile;

        bool is_inside(int i, int j, int k)
        {
            if (   i > x && i < w+x-1
                && j > y && j < h+y-1
                && k != z+l-1
            ) return true;
            return false;
        }

        bool is_window(int i, int j, int k)
        {
            if ( k != z+l-1
             && (k%3) ==2
             && (
                   (i > x && i < w+x-1)
                || (j > y && j < h+y-1))
             && !randrange(0,2)
             ) return true; // windows
            return false;
        }

        void set() {
            int i,j,k;
            for (i=x; i<w+x; i++) {
                for (j=y; j<h+y; j++) {
                    for (k=z; k<z+l; k++) {
                        if (is_inside(i,j,k)) continue;
                        if (is_window(i,j,k)) continue;
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

static const int BUILDING_MAX = 1024;
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


class Road {
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
                        _set(i,j,k,tile);
                    }
                }
            }
        }

        Road(){}
        Road(int id){}
        Road(int x, int y, int z, int w, int h, int l, int tile) :
        x(x), y(y), z(z),
        w(w), h(h), l(l),
        tile(tile)
        {}
};

static const int ROAD_MAX = 32;
class Road_list: public Object_list<Road,ROAD_MAX>
{
    private:
        const char* name() { return "Road"; }

    public:
        void set() {
            int i;
            for (i=0; i<ROAD_MAX; i++) {
                if (a[i] == NULL) continue;
                a[i]->set();
            }
        }

        Road* add(int x, int y, int z, int w, int h, int l, int tile) {
            Road* r = this->create();
            r->x = x;
            r->y = y;
            r->z = z;
            r->w = w;
            r->h = h;
            r->l = l;
            r->tile = tile;
            return r;
        }
};

void l_system();
void generate();

void rect_solver();

//cython
void generate_dragon();
void outline();
}
