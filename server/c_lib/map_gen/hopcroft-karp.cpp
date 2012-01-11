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

class Point {
    public:
    int x,y;
    int bx,by;
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

    bool operator==(const Point& p) {   //EDIT
        if (p.x == this->x && p.y == this->y) return true;  //EDIT
        return false;
    }

    void print() {
        printf("%d %d %d %d\n", x,y,bx,by);
    }

    Point() :
    x(0),y(0),bx(0),by(0)
    {}
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

void swap(int *a, int *b)
{
  int t=*a; *a=*b; *b=t;
}
void quicksort_int(int* points, int beg, int end)
{
  if (end > beg + 1)
  {
    int piv = points[beg];
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (points[l] <= piv)
        l++;
      else
        swap(&points[l], &points[--r]);
    }
    swap(&points[--l], &points[beg]);
    quicksort_int(points, beg, l);
    quicksort_int(points, r, end);
  }
}

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

inline bool get_convex_vertices(int x, int y, int z, int poly_tile, int *q1, int *q2, int *q3, int *q4) {

    if (_get(x,y,z) == poly_tile) return false;

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
    return true;
}

void get_convex_vertices(int z, int poly_tile, Point* pts, int vertex_max, int* points) {
    int p = 0, _p = 0;
    int i,j;
    int q1=0,q2=0,q3=0,q4=0;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            q1=q2=q3=q4=0;
            if (!get_convex_vertices(i,j,z, poly_tile, &q1, &q2, &q3, &q4)) {
                continue;
            }
            _p = p+q1+q2+q3+q4;
            if (_p >= vertex_max) {
                printf("get_convex_vertices Fatal Error -- exceed vertex max %d\n", vertex_max);
                printf("Reached %d,%d\n", i,j);
                *points = 0;
                return;
            }
            if (q1) {   // top left
                pts[p].x = 2*i+1;
                pts[p].y = 2*j+1;
                pts[p].bx = i;
                pts[p].by = j;
                //pts[p].b = j*width + i;
                p++;
            }
            if (q2) {   // bottom left
                pts[p].x = 2*i-1;
                pts[p].y = 2*j+1;
                pts[p].bx = i;
                pts[p].by = j;
                //pts[p].b = j*width + i;
                p++;
            }
            if (q3) {   // bottom right
                pts[p].x = 2*i-1;
                pts[p].y = 2*j-1;
                pts[p].bx = i;
                pts[p].by = j;
                //pts[p].b = j*width + i;
                p++;
            }
            if (q4) {   // top right
                pts[p].x = 2*i+1;
                pts[p].y = 2*j-1;
                pts[p].bx = i;
                pts[p].by = j;
                //pts[p].b = j*width + i;
                p++;
            }
        }
    }
    *points = p;
}

class Diagonal {
    public:
    int p,q;
    int length(Point* pts) {
        Point* _p = &pts[this->p];
        Point* _q = &pts[this->q];
        return sqrt((_q->x - _p->x)*(_q->x - _p->x) + (_q->y - _p->y)*(_q->y - _p->y));
    }
};

void get_vertical_diagonals(Point* pts, int points, Diagonal* diagonals, int diagonals_max, int* diag_ct, int z, int tile) {
    quicksort_pts_x(pts, 0, points);
    int i;
    int* indices = (int*)malloc(sizeof(int)*(width+1));
    for (i=0; i<width; i++) indices[i] = 0;
    for (i=0; i<points;i++) {
        indices[pts[i].x/2]++;
        //indices[pts[i].bx]++;
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
                        printf("get_vertical_diagonals :: Max diagonals reached\n");
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
        //indices[pts[i].by]++;
    }
    indices[height] = points;

    int dct = 0;
    int j,k;
    int off;
    Point *pt,*pt2;
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
                        printf("get_horizontal_diagonals :: Max diagonals reached\n");
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
    Point* top = (v1->y >= v2->y) ? v1 : v2;
    Point* bottom = (v1->y < v2->y) ? v1 : v2;
    Point* left = (h1->x >= h2->x) ? h1 : h2;
    Point* right = (h1->x < h2->x) ? h1 : h2;

    if (v1->x < left->x && v1->x > right->x
     && h1->y < top->y  && h1->y > bottom->y) return true;
    return false;
}
bool orthogonal_intersection_inclusive(Point* v1, Point* v2, Point* h1, Point* h2) {
    Point* top = (v1->y >= v2->y) ? v1 : v2;
    Point* bottom = (v1->y < v2->y) ? v1 : v2;
    Point* left = (h1->x >= h2->x) ? h1 : h2;
    Point* right = (h1->x < h2->x) ? h1 : h2;

    if (v1->x <= left->x && v1->x >= right->x
     && h1->y <= top->y  && h1->y >= bottom->y) return true;
    return false;
}
void find_intersections(Point* p, Diagonal* h, int h_ct, Diagonal* v, int v_ct, Intersection* in, int* i_ct) {
    int i,j;
    int index = 0;
    Point *p1, *p2, *q1, *q2;
    for (i=0; i<v_ct; i++) {
        p1 = &p[v[i].p];
        p2 = &p[v[i].q];
        for (j=0; j<h_ct; j++) {
            q1 = &p[h[j].p];
            q2 = &p[h[j].q];
            if (orthogonal_intersection_inclusive(p1, p2, q1, q2)) {
                in[index].dh = j;
                in[index].dv = i;
                index++;
            }
        }
    }
    *i_ct = index;
    printf("%d intersections found\n", index);
}

void find_intersections(Point* p, Diagonal* d, int d_ct, int h_ct, Intersection* in, int* i_ct) {
    int i,j;
    int index = 0;
    Point *p1, *p2, *q1, *q2;
    for (i=h_ct; i<d_ct; i++) {
        p1 = &p[d[i].p];
        p2 = &p[d[i].q];
        for (j=0; j<h_ct; j++) {
            q1 = &p[d[j].p];
            q2 = &p[d[j].q];
            if (orthogonal_intersection_inclusive(p1, p2, q1, q2)) {
                in[index].dh = j;
                in[index].dv = i;
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
    //FOR(i,1,HK::N+HK::M) {
    FOR(i,0,HK::N+HK::M) {
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
        //printf("%d\n", HK::map[i]);
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

int choose_shortest_diagonal(int h_index, Intersection* in, int i_ct, int h_ct, Diagonal* h_diagonals, Diagonal* v_diagonals, Point* p, int* map) {
    // look in intersections for dh==h_index
    // if multiple matches, find which dv is in matching set map
    // assign ptr to d
    int DEFAULT_V_LEN = 1000000;
    int i;
    int h_len = h_diagonals[h_index].length(p);
    int v_len = DEFAULT_V_LEN;
    int v_len_tmp;
    int v_index;
    int ct = 0;
    for (i=0; i<i_ct; i++) {
        if (in[i].dh == h_index) {
            ct++;
            v_len_tmp = v_diagonals[in[i].dv].length(p);
            if (v_len_tmp < v_len) {
                v_len = v_len_tmp;
                v_index = in[i].dv+h_ct;
            }
            //if (v_len < h_len) return in[i].dv+h_ct;
            //else return h_index;
        }
    }
    if (ct > 1) printf("Found %d possible diags\n", ct);
    if (v_len != DEFAULT_V_LEN) {
        if (v_len < h_len) return v_index;
        else return h_index;
    }
    printf("ERROR No matching diagonal found for diagonal %d\n", h_index);
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
