#pragma once

#include <iostream>
#include <queue>
#include <set>
#include <math.h>

using namespace std;

// Notation for working with points
typedef pair<double, double> point;
#define x first
#define y second

// Arc, event, and segment datatypes
struct arc;
struct seg;

struct event {
   double x;
   point p;
   arc *a;
   bool valid;

   event(double xx, point pp, arc *aa)
      : x(xx), p(pp), a(aa), valid(true) {}
};

struct arc {
   point p;
   arc *prev, *next;
   event *e;

   seg *s0, *s1;

   arc(point pp, arc *a=0, arc *b=0)
    : p(pp), prev(a), next(b), e(0), s0(0), s1(0) {}
};

vector<seg*> output;  // Array of output segments.

struct seg {
   point start, end;
   bool done;

   seg(point p)
      : start(p), end(0,0), done(false)
   { output.push_back(this); }
   
   // Set the end point and mark as "done."
   void finish(point p) { if (done) return; end = p; done = true; }
};

arc *root = 0; // First item in the parabolic front linked list.

// Function declarations
void process_point();
void process_event();
void front_insert(point  p);

bool circle(point a, point b, point c, double *x, point *o);
void check_circle_event(arc *i, double x0);

bool intersect(point p, arc *i, point *result = 0);
point intersection(point p0, point p1, double l);

void finish_edges();
void print_output();

// "Greater than" comparison, for reverse sorting in priority queue.
struct gt {
   bool operator()(point a, point b) {return a.x==b.x ? a.y>b.y : a.x>b.x;}
   bool operator()(event *a, event *b) {return a->x>b->x;}
};

// Bounding box coordinates.
double X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

/* STEVE */
void load_points(point* pts, int n_pts);
void process_points();
/* END STEVE */
#pragma once

#include <stdlib.h>
//#include <libnoise.h>

#include <c_lib/t_map/t_map.hpp>

#include <c_lib/map_gen/voronoi.hpp>

void start_voronoi();
