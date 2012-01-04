#pragma once

#include <iostream>
#include <vector>
#include <queue>

#define FOR(X,Y,Z) for(int X=Y; X<=Z; X++)
#define MAXN 1024

#define pb push_back
#define mp make_pair
#define vi(X) vector< X >::iterator

namespace HK {
using namespace std;

int N, M, map[MAXN], d[MAXN];
vector<int>graph[MAXN];

vector<vector<int> >paths;
vector<int>gm[MAXN];
vector<int>gmp[MAXN];
vector<int>p;

int visited[MAXN];


bool DFS(int v) {
    p.pb(v);
    visited[v]=1;
    if( v>M && !map[v] ) return true;

    for( vi(int) i=gmp[v].begin(); i!=gmp[v].end(); i++) 
        if( !visited[*i] && DFS(*i) ) return true;

    visited[v]=0;
    p.pop_back();
    return false;
}

//BFS
bool MaxPaths() {
    paths.clear();

    // Initial value of O(V)

    FOR(i,1,M+N) {
        visited[i]=0;
        d[i]=10000;
        gm[i].clear();
        gmp[i].clear();
    }
    FOR(i,1,M)  if( !map[i] ) d[i]=0;

    /*  Create a graph mapping gm O(E) */
    FOR(i,1,M)
    for( vi(int) j=graph[i].begin(); j!=graph[i].end(); j++ )
        map[i]==*j ? gm[*j].pb(i) : gm[i].pb(*j);

    /* Calculate distance from the vertices to V1 'O(E) */
    FOR(i,1,M) {
        if( map[i] ) { continue; }
        queue<int>Q; Q.push(i);
        while( !Q.empty() ) {
            int v=Q.front(); Q.pop();
            for( vi(int) j=gm[v].begin(); j!=gm[v].end(); j++) {
                if( d[*j] > d[v]+1 ) {
                    d[*j]=d[v]+1;
                    Q.push(*j);
                }
            }
        }   // so ugly ;]
    }
    FOR(i,1,M+N) // remove unnecessary edge O(E)
    for( vi(int) j=gm[i].begin(); j!=gm[i].end(); j++) {
        if( d[i]+1 == d[*j] ) gmp[i].pb(*j);
    } 
    
    FOR(i,1,M) if( !map[i] ) {
        p.clear();
        if( DFS(i) ) {
            paths.pb( p );
            break;
        }
    }
    if( !paths.empty() ) return true;
    return false;
}

};



int width = 128;
int height = 128;

class Rect {
    public:
    unsigned char ox,oy;
    unsigned char x,y,w,h;
    bool placed;
    int area() {
        return w*h;
    }
    bool collides(Rect* r) {
        return !(x+w > r->x || x < r->x+r->w ||
            y+h > r->y || y < r->y+r->h);
    }
    void place(int z) {
        int new_tile = 7;
        int i,j;
        for (i=x; i<x+w; i++) {
            for (j=y; j<y+h; j++) {
                if (i > x && i < x+w-1 && j > y && j < y+h-1) continue;
                _set(i,j,z+2, new_tile);
            }
        }
        _set(ox,oy,z+2, 1);
        placed = true;
        printf("Placed rect at %d,%d  w,h: %d,%d\n", x,y,w,h);
    }
    Rect() :
    x(0), y(0), w(0), h(0),
    placed(false)
    {}
};
class Point {
    public:
    int x,y;
    bool is_connected_axis_aligned_y(Point* p, int z, int tile) {
        int start,end;
        if (p->y == y) {
            start = (p->x < x) ? p->x : x;
            end = (p->x > x) ? p->x : x;
        } else if (p->x == x) {
            printf("same point\n");
            return false;
        } else {

            printf("%d,%d %d,%d not aligned\n", x,y, p->x, p->y);
            return false;
        }

        int i;
        for (i=start+1; i<end; i++) {
            if (_get(i/2,(y-1)/2,z) != tile) return false;
            if (_get(i/2,(y+1)/2,z) != tile) return false;
        }

        return true;
    }
    
    bool is_connected_axis_aligned_x(Point* p, int z, int tile) {
        int start,end;
        if (p->x == x) {
            start = (p->y < y) ? p->y : y;
            end = (p->y > y) ? p->y : y;
        } else if (p->y == y) {
            printf("same point\n");
            return false;
        } else {
            printf("%d,%d %d,%d not aligned\n", x,y, p->x, p->y);
            return false;
        }

        int i;
        for (i=start+1; i<end; i+=2) {
            if (_get((x-1)/2,i/2,z) != tile) return false;
            if (_get((x+1)/2,i/2,z) != tile) return false;
        }

        return true;
    }
};
bool point_taken(struct Point* pts, int n, struct Point* pt) {
    int i;
    for (i=0; i<n; i++) {
        if (pt->x == pts[i].x && pt->y == pts[i].y) {
            printf("point taken %d,%d\n", pt->x, pt->y);
            return true;
        }
    }
    return false;
}
inline void rotate90(int* x, int *y) {
    int _x = *x;
    *x = -*y;
    *y = _x;
}



/*
A naive implementation of an algorithm that reduces the problem to a bipartite
graph and then applies a general-purpose bipartite graph matching algorithm would
solve the problem in time O(n
2:5
), where n denotes the number of vertices of the input
polygon: this is the time needed to apply the Hopcroft–Karp matching algorithm [41] to
the intersection graph of the good diagonals, which may have Q(n) vertices and Q(n
2
)
edges. However, by using geometric range searching data structures to speed up the
search for alternating paths within the matching algorithm, it is possible to improve the
overall running time to O(n
3=2
logn) [42, 53, 54].
*
* Isolate independent cave networks
* Collect convex vertices
* Connect all axis parallel diagonals between vertices
* Horizontal in one, vertical in another
* Find all intersections between horizontal and vertical; these are graph edges
* Apply Hopcroft-Karp, get maximum matching (edges)
* Using maximum matching, get maximum independent set (vertices aka convex diagonals)
* Draw the maximum independent set
*
* Iterate remaining unconnected convex vertices,
*   connecting to nearest intersection
*/

// quicksort
void swap(Point *a, Point *b)
{
  Point t=*a; *a=*b; *b=t;
}
void quicksort_pts_x(Point* points, int beg, int end)
{
  if (end > beg + 1)
  {
    Point piv = points[beg];
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (points[l].x <= piv.x)
        l++;
      else
        swap(&points[l], &points[--r]);
    }
    swap(&points[--l], &points[beg]);
    quicksort_pts_x(points, beg, l);
    quicksort_pts_x(points, r, end);
  }
}
void quicksort_pts_y(Point* points, int beg, int end)
{
  if (end > beg + 1)
  {
    Point piv = points[beg];
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (points[l].y <= piv.y)
        l++;
      else
        swap(&points[l], &points[--r]);
    }
    swap(&points[--l], &points[beg]);
    quicksort_pts_y(points, beg, l);
    quicksort_pts_y(points, r, end);
  }
}

inline void get_convex_vertices(int x, int y, int z, int poly_tile, int *q1, int *q2, int *q3, int *q4) {

    if (_get(x,y,z) == poly_tile) return;

    int dx=1,dy=0;
    int dx1=0,dy1=1;

    int i;
    for (i=0; i<4; i++) {
        if (_get(x+dx, y+dy, z) == poly_tile
         && _get(x+dx1, y+dy1, z) == poly_tile
         && _get(x+dx+dx1, y+dy+dy1, z) == poly_tile)
        {
            switch(i) {
                case 0:
                    *q1 = 1;
                    break;
                case 1:
                    *q2 = 1;
                    break;
                case 2:
                    *q3 = 1;
                    break;
                case 3:
                    *q4 = 1;
                    break;
                default:
                    break;
            }
        }
        rotate90(&dx,&dy);
        rotate90(&dx1,&dy1);
    }
}

void get_convex_vertices(int z, int poly_tile, Point* pts, int vertex_max, int* points) {
    int p = 0, _p = 0;
    int i,j;
    int q1=0,q2=0,q3=0,q4=0;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            get_convex_vertices(i,j,z, poly_tile, &q1, &q2, &q3, &q4);
            _p = p+q1+q2+q3+q4;
            if (_p >= vertex_max) {
                printf("get_convex_vertices Fatal Error -- exceed vertex max %d\n", vertex_max);
                printf("Reached %d,%d\n", i,j);
                *points = 0;
                return;
            }
            if (q1) {
                pts[p].x = 2*i+1;
                pts[p].y = 2*j+1;
                p++;
            }
            if (q2) {
                pts[p].x = 2*i-1;
                pts[p].y = 2*j+1;
                p++;
            }
            if (q3) {
                pts[p].x = 2*i-1;
                pts[p].y = 2*j-1;
                p++;
            }
            if (q4) {
                pts[p].x = 2*i+1;
                pts[p].y = 2*j-1;
                p++;
            }
            q1=q2=q3=q4=0;
        }
    }
    *points = p;
}

class Diagonal {
    public:
    int p,q;
    int length(Point* p1) {
        Point* _p = &p1[p];
        Point* _q = &p1[q];
        return sqrt((_q->x - _p->x)*(_q->x - _p->x) +(_q->y - _p->y)*(_q->y - _p->y));
    }
};

void get_vertical_diagonals(Point* pts, int points, Diagonal* diagonals, int diagonals_max, int* diag_ct, int z, int tile) {
    quicksort_pts_x(pts, 0, points);
    int i;
    int* indices = (int*)malloc(sizeof(int)*(width+1));
    for (i=0; i<width; i++) indices[i] = 0;
    for (i=0; i<points;i++) {
        indices[pts[i].x/2]++;
    }
    indices[width] = points;

    int dct = 0;
    int j,k;
    int off;
    Point* pt,*pt2;
    int index = 0;
    for (i=0; i<width; i++) {
        off = 0;
        for (j=0; j<indices[i]; j++) {
            pt = &pts[j+index];
            for (k=off; k<indices[i]; k++) {
                if (j==k) continue;
                pt2 = &pts[k+index];
                if (pt->is_connected_axis_aligned_x(pt2, z, tile)) {
                    if (dct > diagonals_max) {
                        printf("Max diagonals reached\n");
                        return;
                    }
                    diagonals[dct].p = j+index;
                    diagonals[dct].q = k+index;
                    dct++;
                }
            }
            off++;
        }
        index += indices[i];
    }
    *diag_ct = dct;
    free(indices);
}

void get_horizontal_diagonals(Point* pts, int points, Diagonal* diagonals, int diagonals_max, int* diag_ct, int z, int tile) {
    quicksort_pts_y(pts, 0, points);
    int i;
    int* indices = (int*)malloc(sizeof(int)*(height+1));
    for (i=0; i<height; i++) indices[i] = 0;
    for (i=0; i<points;i++) {
        indices[pts[i].y/2]++;
    }
    indices[height] = points;

    int dct = 0;
    int j,k;
    int off;
    Point* pt,*pt2;
    int index = 0;
    for (i=0; i<height; i++) {
        off = 0;
        for (j=0; j<indices[i]; j++) {
            pt = &pts[j+index];
            for (k=off; k<indices[i]; k++) {
                if (j==k) continue;
                pt2 = &pts[k+index];
                if (pt->is_connected_axis_aligned_y(pt2, z, tile)) {
                    if (dct > diagonals_max) {
                        printf("Max diagonals reached\n");
                        return;
                    }
                    diagonals[dct].p = j+index;
                    diagonals[dct].q = k+index;
                    dct++;
                }
            }
            off++;
        }
        index += indices[i];
    }
    *diag_ct = dct;
    free(indices);
}

class Intersection {
    public:
    int dh,dv;
};
bool orthogonal_intersection(Point* v1, Point* v2, Point* h1, Point* h2) {
    Point* top = (v1->y > v2->y) ? v1 : v2;
    Point* bottom = (v1->y < v2->y) ? v1 : v2;
    Point* left = (h1->x > h2->x) ? h1 : h2;
    Point* right = (h1->x < h2->x) ? h1 : h2;
    //printf("%d %d %d %d\n", v1->x, v1->y, v2->x, v2->y);
    //printf("%d %d %d %d\n", h1->x, h1->y, h2->x, h2->y);
    //printf("\n");
    if (v1->x < left->x && v1->x > right->x
     && h1->y < top->y  && h1->y > bottom->y) return true;
    return false;
}
void find_intersections(Point* hp, Diagonal* h, int h_ct, Point* vp, Diagonal* v, int v_ct, Intersection* in, int* i_ct) {
    int i,j;
    int index = 0;
    Point *p1, *p2, *q1, *q2;
    for (i=0; i<v_ct; i++) {
        p1 = &vp[v[i].p];
        p2 = &vp[v[i].q];
        for (j=0; j<h_ct; j++) {
            q1 = &hp[h[j].p];
            q2 = &hp[h[j].q];
            if (orthogonal_intersection(p1, p2, q1, q2)) {
                in[index].dh = i;
                in[index].dv = j;
                index++;
            }
        }
    }
    *i_ct = index;
    printf("%d intersections found\n", index);
}

int hk(int h_ct, int v_ct, int i_ct, Intersection* intersections) {
    HK::M = h_ct;
    HK::N = v_ct;

    // init graph
    FOR(i,1,HK::N+HK::M) {
        HK::graph[i].clear();
        HK::map[i]=0;
    }

    // load graph
    for (int j=0; j<i_ct; j++) {
        Intersection* inter = &intersections[j];
        HK::graph[inter->dh+1].pb(inter->dv+HK::M+1);
        HK::graph[inter->dv+HK::M+1].pb(inter->dh+1);
    }

    while( HK::MaxPaths() ) {   // until Magnifying paths are growing
        for(std::vi(std::vector<int>) i=HK::paths.begin(); i!=HK::paths.end(); i++)
            for(std::vi(int) j=i->begin(); j!=i->end(); j+=2) {
                HK::map[ HK::map[*j] ] = 0;
                HK::map[*j] = *(j+1);
                HK::map[ HK::map[*(j+1)] ] = 0;
                HK::map[*(j+1)] = *j;
            }                   
    }
    int ile=0;
    FOR(i,1,HK::M) {
        if( HK::map[i] ) ile++;
    }
    printf("Maximal matching cardinality: %d\n", ile);
    printf("M+N: %d\n", HK::M+HK::N);
    printf("Edge count: %d\n", i_ct);
    return ile;
}

int find_matching_diagonal(int h_index, Intersection* in, int i_ct, int h_ct, int* map) {
    // look in intersections for dh==h_index
    // if multiple matches, find which dv is in matching set map
    // assign ptr to d
    int i;
    bool h_found = false;
    for (i=0; i<i_ct; i++) {
        if (in[i].dh == h_index) {
            h_found = true;
            if (map[1+h_ct+in[i].dv]) {
                return in[i].dv;
            }
        }
    }
    printf("No matching diagonal found\n");
    if (h_found) printf("h %d was found\n", h_index);
    else printf("h %d not found\n", h_index);
    return -1;
}

int choose_shortest_diagonal(int h_index, Intersection* in, int i_ct, int h_ct, Diagonal* h_diagonals, Diagonal* v_diagonals, Point* p1, Point* p2, int* map) {
    // look in intersections for dh==h_index
    // if multiple matches, find which dv is in matching set map
    // assign ptr to d
    int i;
    int h_len = h_diagonals[h_index].length(p1);
    int v_len;
    for (i=0; i<i_ct; i++) {
        if (in[i].dh == h_index) {
            v_len = v_diagonals[in[i].dv].length(p2);
            if (v_len < h_len) return in[i].dv+h_ct;
            else return h_index;
        }
    }
    printf("No matching diagonal found\n");
    return -1;
}

/*
 * Debugging final results: (some obvious omissions from independent set, diagonals that had no intersection)
 * -Missing diagonals exist in initial set. diagonal processing likely correct.
 *
 * Remaining possibilities:
 * -Input to HK algo wrong.
 * -HK algo implementation wrong
 * -Maximum matching -> maximum independent set process wrong
 *
 */
