#include "dungeon.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/map_gen/features.h>
#include <math.h>
#include <c_lib/common/random.h>

#include "hopcroft-karp.cpp"


namespace Dungeon {

class Hole {
    public:
    unsigned char h,w;
    int x,y;
    int tile;
    void restore(int z) {
        int i,j;
        for (i=x; i<x+w; i++) {
            for(j=y; j<y+h; j++) {
                _set(i,j,z,this->tile);
            }
        }
    }
};

int max_holes = 256;
Hole* holes;
int n_holes = 0;

void save_and_remove_holes(int z, int tile) {
    Hole* h = (Hole*)malloc(sizeof(Hole)*max_holes);
    int n=0;

    // raster for holes
    int i,j;
    int tmp_tile;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            tmp_tile = _get(i,j,z);
            if (tmp_tile != tile) {
                if (_get(i+1, j,z)==tile
                 && _get(i-1, j,z)==tile
                 && _get(i, j+1,z)==tile
                 && _get(i, j-1,z)==tile)
                {
                    h[n].w = 1;
                    h[n].h = 1;
                    h[n].x = i;
                    h[n].y = j;
                    h[n].tile = tmp_tile;
                    n++;
                    _set(i,j,z,tile);
                }
            }
        }
    }

    n_holes = n;
    h = (Hole*)realloc(h, sizeof(Hole)*n);
    holes = h;
}

void restore_saved_holes(int z) {
    int i;
    for (i=0; i<n_holes;i++) {
        holes[i].restore(z);
    }
}

inline bool point_in_set(Point p, Diagonal* loose_ends, int n_ends, Point* pts) {
    int i;
    Diagonal* diag;
    for (i=0; i<n_ends; i++) {
        diag = &loose_ends[i];
        if (pts[diag->p] == p || pts[diag->q] == p) return true;
    }
    return false;
}

inline int point_in_points(Point p, Point* pts, int points) {
    int i;
    for (i=0; i<points; i++) {
        if (pts[i] == p) return i;
    }
    return -1;
}
inline int point_in_points_both(Point p, Point* pts, int points) {
    int i;
    for (i=0; i<points; i++) {
        if (pts[i] == p && pts[i].bx == p.bx && pts[i].by == p.by) return i;
    }
    return -1;
}

inline bool point_intersects_diagonal(int x, int y, Diagonal* loose_ends, int n_ends, Point* pts) {
    int i;
    Diagonal* d;
    Point *p,*q;
    int start_x, end_x, start_y, end_y;
    for (i=0; i<n_ends; i++) {
        d = &loose_ends[i];
        p = &pts[d->p];
        q = &pts[d->q];
        start_x = (p->x >= q->x) ? q->x : p->x;
        end_x = (p->x < q->x) ? q->x : p->x;
        start_y = (p->y >= q->y) ? q->y : p->y;
        end_y = (p->y < q->y) ? q->y : p->y;
        if (x >= start_x && x <= end_x && y >= start_y && y <= end_y) return true;
    }
    return false;
}

Point get_nearest_endpoint(Point _p, Diagonal* loose_ends, int n_ends, Point* pts, int z, int tile) {
    Point *p = &_p;
    Point q;

    int dx,dy;
    int x=0,y=0;
    dx = p->x - 2*p->bx;
    dy = p->y - 2*p->by;
    while(1) {
        x += dx;
        y += dy;
        if (_get((p->x + x)/2, (p->y+1)/2, z) != tile || _get((p->x + x)/2, (p->y-1)/2, z) != tile) // another vertex
        {
            q.x = p->x+x;
            if (x>0) q.x-=1;
            q.y = p->y;
            break;
        }
        if (point_intersects_diagonal(p->x+x,p->y, loose_ends, n_ends, pts))      // intersection with existing diagonal
        {
            q.x = p->x+x;
            q.y = p->y;
            break;
        }
        if (_get((p->x+1)/2, (p->y+y)/2, z) != tile || _get((p->x-1)/2, (p->y+y)/2, z) != tile) // another vertex
        {
            q.x = p->x;
            q.y = p->y+y;
            if (y>0) q.y-=1;
            break;
        }
        if (point_intersects_diagonal(p->x,p->y+y, loose_ends, n_ends, pts))      // intersection with existing diagonal
        {
            q.x = p->x;
            q.y = p->y+y;
            break;
        }
        if (((p->x+x)/2 > width || p->x+x < 0) && ((p->y+y)/2 > height || p->y+y < 0))
        {
            printf("ERROR get_nearest_endpoint left the map\n");
            break;
        }
    }
    return q;
}

void renumber_v_diagonals(Diagonal *v_diagonals, int v_diag_ct, Point* pts2, Point* pts, int points) {
    bool p_found,q_found;
    int i,j;
    Diagonal *d;
    Point p;
    for (i=0; i<v_diag_ct; i++) {
        p_found = false;
        q_found = false;
        d = &v_diagonals[i];
        
        p = pts2[d->p];
        for (j=0; j<points; j++) {
            if (p == pts[j]) {
                d->p = j;
                p_found = true;
                break;
            }
        }

        p = pts2[d->q];
        for (j=0; j<points; j++) {
            if (p == pts[j]) {
                d->q = j;
                q_found = true;
                break;
            }
        }

        if (!p_found) printf("d->p not found in pts.\n");
        if (!q_found) printf("d->p not found in pts.\n");
    }
}

void swap_diag(Diagonal *a, Diagonal *b)
{
  Diagonal t=*a; *a=*b; *b=t;
}
void quicksort_diagonals_HV(Diagonal* d, Point* p, int beg, int end)
{
    bool is_h = false;
  if (end > beg + 1)
  {
    Diagonal diag = d[beg];
    if (p[diag.p].y == p[diag.q].y) is_h = true;
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (p[d[l].p].y == p[d[l].q].y && !is_h)
        l++;
      else
        swap_diag(&d[l], &d[--r]);
    }
    swap_diag(&d[--l], &d[beg]);
    quicksort_diagonals_HV(d, p, beg, l);
    quicksort_diagonals_HV(d, p, r, end);
  }
}

class Room {
    public:
    int x,y,w,h;
    int z,d;
    bool placed;
    int edges;
    int clique;
    int area() {
        return w*h;
    }

    bool collides(Room* r) {
        return !(x+w > r->x || x < r->x+r->w ||
            y+h > r->y || y < r->y+r->h);
    }

    void draw(int z, int tile) {
        int i,j;
        for (i=x-w; i<x; i++) {
            for (j=y; j<y+h; j++) {
                if (z>=zmax) printf("Room -- clique %d\n", clique);
                _set(i,j,z,tile);
            }
        }
    }

    void draw_3d(int tile) {
        int k;
        for (k=z; k<z+d; k++) {
            this->draw(k, tile);
        }
    }

    void print() {
        printf("%d %d %d %d\n", x,y,w,h);
    }

    bool shares_edge_with(Room* r) {
        // horizontal edges
        if (y == r->y || y == r->y+r->h || y+h == r->y || y+h == r->y+r->h) {
            if (x < r->x && x > r->x-r->w) return true;
            if (x-w < r->x && x-w > r->x-r->w) return true;
            if (x == r->x && w == r->w) return true;
        }
        // vertical edges
        if (x == r->x || x == r->x-r->w || x-w == r->x || x-w == r->x-r->w) {
            if (y > r->y && y < r->y+r->h) return true;
            if (y+h > r->y && y+h < r->y+r->h) return true;
            if (y == r->y && h == r->h) return true;
        }

        return false;
    }

    float center_fx() {
        return ((float)x) - ((float)w)/2.0f;
    }

    float center_fy() {
        return ((float)y) + ((float)h)/2.0f;
    }

    Room() :
    x(0), y(0), w(0), h(0),
    z(0), d(0),
    placed(false),
    edges(0)
    {}
};

inline bool is_topleft_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x+1,y,z) == tile) return false;
    if (_get(x,y-1,z) == tile) return false;
    if (_get(x+1,y-1,z) == tile) return false;
    return true;
}
inline bool is_topright_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x-1,y,z) == tile) return false;
    if (_get(x,y-1,z) == tile) return false;
    if (_get(x-1,y-1,z) == tile) return false;
    return true;
}
inline bool is_bottomleft_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x+1,y,z) == tile) return false;
    if (_get(x,y+1,z) == tile) return false;
    if (_get(x+1,y+1,z) == tile) return false;
    return true;
}
inline bool is_bottomright_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x-1,y,z) == tile) return false;
    if (_get(x,y+1,z) == tile) return false;
    if (_get(x-1,y+1,z) == tile) return false;
    return true;
}
inline bool is_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    
    if(is_topleft_corner(x,y,z,tile)) return true;
    if(is_topright_corner(x,y,z,tile)) return true;
    if(is_bottomleft_corner(x,y,z,tile)) return true;
    if(is_bottomright_corner(x,y,z,tile)) return true;
    return false;
}

inline bool point_is_leftmost_horizontal_endpoint(Point p, Point* points, Diagonal* diagonals, int n_diagonals, int n_horizontals) {
    int i;
    Diagonal *d;
    Point smaller;
    Point r,s;
    for (i=0; i<n_horizontals; i++) {
        d = &diagonals[i];
        r = points[d->p];
        s = points[d->q];
        smaller = (r.x > s.x) ? r : s;
        if (p == smaller) return true;
    }
    return false;
}

inline bool point_is_bottommost_vertical_endpoint(Point p, Point* points, Diagonal* diagonals, int n_diagonals, int n_horizontals) {
    int i;
    Diagonal *d;
    Point higher;
    Point r,s;
    for (i=n_horizontals; i<n_diagonals; i++) {
        d = &diagonals[i];
        r = points[d->p];
        s = points[d->q];
        higher = (r.y < s.y) ? r : s;
        if (p == higher) return true;
    }
    return false;
}

inline bool is_vertical_line_segment(int tx, int ty, int bx, int by, Point* points, Diagonal *diagonals, int n_diagonals,  int n_horizontals) {
    // tx,ty must be >= bx,by
    int i;
    Point *p,*q;
    Diagonal* d;
    Point *top,*bottom;
    for (i=n_horizontals; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];
        if (p->x != tx) continue;
        top = (p->y > q->y) ? p : q;
        bottom = (p->y < q->y) ? p : q;
        if (ty <= top->y && by >= bottom->y) return true;
    }
    return false;
}


inline bool is_horizontal_line_segment(int lx, int ly, int rx, int ry, Point* points, Diagonal *diagonals, int n_diagonals,  int n_horizontals) {
    // lx,ly must be >= rx,ry
    int i;
    Point *p,*q;
    Diagonal* d;
    Point *left,*right;
    for (i=0; i<n_horizontals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];
        if (p->y != ly) continue;
        left = (p->x >= q->x) ? p : q;
        right = (p->x < q->x) ? p : q;
        if (lx <= left->x && rx >= right->x) return true;
    }
    return false;
}

void find_duplicate_corners(Point* corners, int n_corners) {
    int i,j;
    int n = 0;
    Point *p,*q;
    for (i=0; i<n_corners; i++) {
        p = &corners[i];
        for (j=0; j<n_corners; j++) {
            if (i==j) continue;
            q = &corners[j];
            if (p->x == q->x && p->y == q->y && p->bx == q->bx && p->by == q->by) {
                //p->print();
                n++;
                _set(p->bx, p->by, 13, 1);
            }
        }
    }
    if (n) printf("ERROR: %d duplicate corners found.\n", n);
}

void remove_corner(int id, Point* corners, int* n_corners) {
    Point* p;
    p = &corners[id];
    p->x = 0;
    p->y = 0;
    p->bx = 0;
    p->by = 0;
    
    int i;
    for (i=id; i<*n_corners-1; i++) {
        corners[i] = corners[i+1];
    }
    *n_corners -= 1;
}

void remove_duplicate_corners(Point* corners, int* n_corners) {
    int i,j;
    Point *p,*q;
    int n_dupes = 0;
    Point* dupes = (Point*)malloc(sizeof(Point)*(*n_corners/2));
    int* dupe_ids = (int*)malloc(sizeof(int)*(*n_corners/2));
    for (i=0; i<*n_corners; i++) {
        p = &corners[i];
        for (j=0; j<*n_corners; j++) {
            if (i==j) continue;
            q = &corners[j];
            if (p->x == q->x && p->y == q->y && p->bx == q->bx && p->by == q->by) {
                if (point_in_points_both(*p, dupes, n_dupes) < 0) {
                    dupes[n_dupes] = *p;
                    dupe_ids[n_dupes] = j;
                    n_dupes++;
                }
            }
        }
    }

    quicksort_int(dupe_ids, 0, n_dupes);

    int id;
    for (i=n_dupes-1; i>=0; i--) {
        id = dupe_ids[i];
        remove_corner(id, corners, n_corners);
    }

    printf("removed %d corners\n", n_dupes);
    
    free(dupes);
    free(dupe_ids);
}

Point *points = NULL;
int n_points = 0;
Diagonal* diagonals = NULL;
int n_diagonals = 0;
int n_horizontals = 0;

Point *corners = NULL;
int n_bottom_corners = 0;
int n_corners = 0;

Room* rooms = NULL;
int n_rooms = 0;
int n_room_edges = 0;

void rect_solver(int z) {
    if (points != NULL) {
        free(points);
        points = NULL;
        n_points = 0;
    }
    if (diagonals != NULL) {
        free(diagonals);
        diagonals = NULL;
        n_diagonals = 0;
        n_horizontals = 0;
    }
    
    int tile = 103; // solar panel

    // temporarily remove 2x2 or smaller "holes" from map
    save_and_remove_holes(z, tile);

    int vertex_max = 1024*4;
    Point* pts = (Point*)malloc(sizeof(Point)*vertex_max);

    int i;
    for (i=0; i<vertex_max; i++) {
        pts[i].x = 0;
        pts[i].y = 0;
        pts[i].bx = 0;
        pts[i].by = 0;
    }

    n_points=0;
    get_convex_vertices(z, tile, pts, vertex_max, &n_points);
    if (n_points > vertex_max) printf("ERROR: n_points %d exceeds vertex_max %d\n", n_points, vertex_max);
    printf("Got %d vertices\n", n_points);

    if (!n_points) {
        free(pts);
        return;
    }

    pts = (Point*)realloc(pts, sizeof(Point)*n_points);
    Point* pts2 = (Point*)malloc(sizeof(Point)*n_points);
    memcpy(pts2, pts, sizeof(Point)*n_points);
    for (i=0; i<n_points;i++) {
        if (!(pts[i].x%2) || !(pts[i].y%2)) printf("WARNING Even point: %d :: %d %d\n", i, pts[i].x, pts[i].y);
    }

    int h_diag_ct,v_diag_ct;
    int diagonals_max = 2048*4;
    Diagonal* h_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    get_horizontal_diagonals(pts, n_points, h_diagonals, diagonals_max, &h_diag_ct, z, tile);
    h_diagonals = (Diagonal*)realloc(h_diagonals, sizeof(Diagonal)*h_diag_ct);

    Diagonal* v_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    get_vertical_diagonals(pts2, n_points, v_diagonals, diagonals_max, &v_diag_ct, z, tile);
    v_diagonals = (Diagonal*)realloc(v_diagonals, sizeof(Diagonal)*v_diag_ct);

    n_diagonals = h_diag_ct + v_diag_ct;
    renumber_v_diagonals(v_diagonals, v_diag_ct, pts2, pts, n_points);
    free(pts2);

    // find intersections
    int i_ct = 0;
    Intersection* intersections = (Intersection*)malloc(sizeof(Intersection)*v_diag_ct*h_diag_ct);
    find_intersections(pts, h_diagonals, h_diag_ct, v_diagonals, v_diag_ct, intersections, &i_ct);
    intersections = (Intersection*)realloc(intersections, sizeof(Intersection)*i_ct);

    // apply Hopcropt-Karp to the bipartite graph (Horizontals, Verticals, Intersections)
    int n_matches = hk(h_diag_ct, v_diag_ct, i_ct, intersections);

    // get a maximum independent vertex (diagonals) set from hk results
    int* independent_set = (int*)malloc(sizeof(int)*(n_diagonals));
    int n_ind = 0;
    int k;
    int h_len,v_len;
    for (i=0; i<h_diag_ct; i++) {
        if (!HK::map[i+1]) {  // free h_diag vertex
            independent_set[n_ind++] = i;
        } else {
            h_len = h_diagonals[i].length(pts);
            v_len = v_diagonals[HK::map[i+1] - h_diag_ct - 1].length(pts);
            //if (v_len < h_len) k = HK::map[i+1] - h_diag_ct - 1;
            if (v_len < h_len) k = HK::map[i+1] - 1;
            else k = i;
            independent_set[n_ind++] = k;
        }
    }

    for (i=h_diag_ct; i<n_diagonals; i++) {
        if (!HK::map[i+1]) {
            independent_set[n_ind++] = i;
        }
    }

    printf("Independent Diagonals: %d\n", n_ind);
    if (n_ind != n_diagonals-n_matches) {
        printf("ERROR: independent diagonals processed wrong. Should be %d\n", (n_diagonals)-n_matches);
    }

    /*
     *  Connect remaining vertices
     */

    // copy diagonals into one array, with room for new diagonals
    int n_ends_max = n_points * 2;
    int index;
    diagonals = (Diagonal*)malloc(sizeof(Diagonal)*(n_points));
    for (i=0; i<n_ind; i++) {
        index = independent_set[i];
        if (index < h_diag_ct) {
            diagonals[i] = h_diagonals[index];
        } else {
            diagonals[i] = v_diagonals[index-h_diag_ct];
        }
    }

    free(independent_set);
    
    pts = (Point*)realloc(pts, sizeof(Point)*(n_points+n_ends_max));
    int n_ends = 0;
    Point p;
    Diagonal* d;
    Point new_p;
    int new_p_slot;
    for (i=0; i<n_points; i++) {
        p = pts[i];
        if (point_in_set(p, diagonals, n_ends+n_ind, pts)) continue;
        d = &diagonals[n_ind+n_ends];
        d->p = i;
        new_p = get_nearest_endpoint(p, diagonals, n_ind+n_ends, pts, z, tile);   // find new endpoint
        new_p_slot = point_in_points(new_p, pts, n_points+n_ends);
        if (new_p_slot < 0) {   // point not in pts
            new_p_slot = n_points+n_ends;
            pts[new_p_slot] = new_p;     // add new point
        }
        d->q = new_p_slot;
        n_ends++;
        if (n_ends >= n_ends_max) {
            printf("n_ends_max %d reached.\n", n_ends_max);
            break;
        }
    }

    pts = (Point*)realloc(pts, sizeof(Point)*(n_points+n_ends));
    points = pts;

    n_diagonals = n_ind + n_ends;
    diagonals = (Diagonal*)realloc(diagonals, sizeof(Diagonal)*n_diagonals);

    // sort diagonals by horizontal-vertical
    // save h_diag_ct;
    quicksort_diagonals_HV(diagonals, points, 0, n_diagonals);
    n_horizontals = 0;
    Point *pp, *qq;
    for (i=0; i<n_diagonals; i++) {
        pp = &points[diagonals[i].p];
        qq = &points[diagonals[i].q];
        if (pp->x == qq->x) // reached verticals
        {
            n_horizontals = i-1;
            break;
        }
    }

    restore_saved_holes(z);
}

void resolve_rooms(int z, int tile) {

    n_bottom_corners = 0;
    n_corners = 0;
    if (corners != NULL) {
        free(corners);
        corners = NULL;
    }
    int corners_max = 2048*4;
    corners = (Point*)malloc(sizeof(Point)*corners_max);

    n_rooms = 0;
    if (rooms != NULL) {
        free(rooms);
        rooms = NULL;
    }

    //if (!n_points) return;

    int i,j;
    Point *p,*q;
    Diagonal *d;

    int bx,by;
    int dx,dy;
    // iterate diagonals
    // look for bottom or left point intersection with block
    for (i=0; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];

        if (p->y == q->y)        // left
        {
            p = (p->x > q->x) ? p : q;
            bx = (p->x - 1)/2;
            by = (p->y - 1)/2;
            if (_get(bx, by+1, z) == tile
             && _get(bx+1, by+1, z) != tile)            // corner
            {
                q = &corners[n_corners++];
                q->bx = bx;
                q->by = by+1;
                q->x = p->x;
                q->y = p->y;
            }
        }
        else if (p->x == q->x)  // bottom
        {
            p = (p->y < q->y) ? p : q;
            bx = (p->x - 1)/2;
            by = (p->y - 1)/2;
            if (_get(bx, by+1, z) == tile
             && _get(bx, by, z) != tile)            // corner
            {
                q = &corners[n_corners++];
                q->bx = bx;
                q->by = by+1;
                q->x = p->x;
                q->y = p->y;
            }
        }
        
    }

    // iterate for tile patterns
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            if (!is_topleft_corner(i,j,z, tile)) continue;
            // save corner
            p = &corners[n_corners++];
            p->bx = i;
            p->by = j;
            p->x = 2*i+1;
            p->y = 2*j-1;
        }
    }

    // find all intersections between diagonals (endpoint inclusive)
    int intersect_max = (n_horizontals)*(n_diagonals-n_horizontals);
    int i_ct = 0;
    Intersection* intersections = (Intersection*)malloc(sizeof(Intersection)*intersect_max);    
    find_intersections(points, diagonals, n_diagonals, n_horizontals, intersections, &i_ct);
    intersections = (Intersection*)realloc(intersections, sizeof(Intersection)*i_ct);

    Intersection* in;
    Diagonal *e;
    Point *top,*left;
    Point *right,*bottom;
    int x,y;
    for (i=0; i<i_ct; i++) {
        in = &intersections[i];

        d = &diagonals[in->dh];
        e = &diagonals[in->dv];

        top = (points[e->p].y > points[e->q].y) ? &points[e->p] : &points[e->q];
        right = (points[d->p].x < points[d->q].x) ? &points[d->p] : &points[d->q];

        if (top->y - right->y < 2) continue;
        if (top->x - right->x < 2) continue;

        x = top->x;
        y = right->y;

        p = &corners[n_corners++];
        p->x = x;
        p->y = y;
        p->bx = (x-1)/2;
        p->by = (y-1)/2 + 1;
    }

    n_bottom_corners = n_corners;

    if (n_corners > corners_max) printf("ERROR: n_corners %d exceeds corners_max %d\n", n_corners, corners_max);

    // get the rest of the corners

    // all the intersection points are corners
    Point qq;
    for (i=0; i<i_ct; i++) {
        in = &intersections[i];

        d = &diagonals[in->dh];
        e = &diagonals[in->dv];

        qq.x = points[e->p].x;
        qq.y = points[d->p].y;

        // intersection, but not endpoint inclusive. 4way intersection
        if (orthogonal_intersection(&points[e->p], &points[e->q], &points[d->p], &points[d->q])) {
            
            qq.bx = (qq.x-1)/2;
            qq.by = (qq.y-1)/2;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;

            qq.bx = (qq.x-1)/2+1;
            qq.by = (qq.y-1)/2;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;

            qq.bx = (qq.x-1)/2+1;
            qq.by = (qq.y-1)/2+1;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;
            
        } else {    // endpoint intersects

            right = (points[d->p].x > points[d->q].x) ? &points[d->p] : &points[d->q];
            left = (points[d->p].x < points[d->q].x) ? &points[d->p] : &points[d->q];
            top = (points[e->p].y > points[e->q].y) ? &points[e->p] : &points[e->q];
            bottom = (points[e->p].y < points[e->q].y) ? &points[e->p] : &points[e->q];

            bool right_edge = false;
            bool left_edge = false;
            bool top_edge = false;
            bool bottom_edge = false;

            if (right->x == top->x) left_edge = true;   // -|
            else
            if (left->x == top->x) right_edge = true;   // |-
            else
            if (top->y == left->y) bottom_edge = true;  // T
            else
            if (bottom->y == left->y) top_edge = true;  // ⟂

            if (!(left_edge || right_edge || top_edge || bottom_edge)) {
                left->print();
                right->print();
                top->print();
                bottom->print();
                printf("WTF\n");
            }

            if (left_edge) { // -|
                //printf("left edge\n");
                qq.bx = (qq.x-1)/2;
                qq.by = (qq.y-1)/2;
                //_set(qq.bx, qq.by, z, 1);
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (right_edge) { // |-
                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2+1;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;

                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (bottom_edge) {  // T
                qq.bx = (qq.x-1)/2;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;

                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (top_edge) {     // ⟂
                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2+1;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
        }
    }

    free(intersections);

    // tile patterns
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            if (_get(i,j,z) != tile) continue;

            if (is_topright_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i-1;
                p->y = 2*j-1;
            }
            if (is_bottomright_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i-1;
                p->y = 2*j+1;
            }
            if (is_bottomleft_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i+1;
                p->y = 2*j+1;
            }
        }
    }

    //// diagonal-wall patterns
    Point *pp;
    for (i=0; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];

        if (i < n_horizontals) {
            right = (p->x >= q->x) ? p : q;
            left = (p->x < q->x) ? p : q;

            //      |-
            bx = (left->x - 1)/2;
            by = (left->y - 1)/2;
            if (_get(bx+1, by, z) == tile
             && _get(bx, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by;
                pp->x = left->x;
                pp->y = left->y;
            }

            if (_get(bx+1, by+1, z) == tile
             && _get(bx, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by+1;
                pp->x = left->x;
                pp->y = left->y;
            }

            //      -|
            bx = (right->x - 1)/2;
            by = (right->y - 1)/2;
            if (_get(bx, by, z) == tile
             && _get(bx+1, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx;
                pp->by = by;
                pp->x = right->x;
                pp->y = right->y;
            }

        } else {
            top = (p->y >= q->y) ? p : q;
            bottom = (p->y < q->y) ? p : q;

            //      T
            bx = (top->x - 1)/2;
            by = (top->y - 1)/2;
            if (_get(bx, by, z) == tile
             && _get(bx, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx;
                pp->by = by;
                pp->x = top->x;
                pp->y = top->y;
            }
            
            if (_get(bx+1, by, z) == tile
             && _get(bx+1, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by;
                pp->x = top->x;
                pp->y = top->y;
            }

            //      ⟂
            bx = (bottom->x - 1)/2;
            by = (bottom->y - 1)/2;
            if (_get(bx+1, by+1, z) == tile
             && _get(bx+1, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by+1;
                pp->x = bottom->x;
                pp->y = bottom->y;
            }

        }
    }

    if (n_corners > corners_max) printf("ERROR: n_corners %d exceeds corners_max %d\n", n_corners, corners_max);

    remove_duplicate_corners(corners, &n_corners);

    if (n_corners > corners_max) printf("ERROR: n_corners %d exceeds corners_max %d\n", n_corners, corners_max);

    corners = (Point*)realloc(corners, sizeof(Point)*n_corners);

    rooms = (Room*)malloc(sizeof(Room) * n_bottom_corners);

    Room *r;
    n_rooms = 0;
    int k;
    int p_index;
    for (i=0; i<n_bottom_corners;i++) {
        r = &rooms[n_rooms++];
        p = &corners[i];
        r->x = (p->x)/2 + 1;
        r->y = (p->y)/2 + 1;
        qq = *p;
        // trace from here
        dx = 0;
        dy = 1;
        k = 0;
        do {
            qq.x += dx;
            qq.y += dy;
            if (dx) {
                qq.bx = qq.x/2 + 1;
                qq.by = qq.y/2;
            }
            if (dy) {
                qq.bx = qq.x/2;
                qq.by = qq.y/2;
            }

            if (qq.x<0||qq.x>=width*2||qq.y<0||qq.y>=height*2) {
                printf("ERROR room trace out of bounds\n");
                if (pp == NULL) {
                    printf("Never found a point\n");
                } else {
                    pp->print();
                }
                n_rooms--;
                break;
            }
            p_index = point_in_points_both(qq, corners, n_corners);
            if (p_index >= 0) {
                pp = &corners[p_index];
                if (dx) {
                    if (pp->bx+1 != (qq.x)/2 && pp->by != (qq.y)/2) {
                        continue;
                    }
                    r->w = (p->x - qq.x)/2;
                }
                if (dy) {
                    if (pp->bx != (qq.x)/2 && pp->by != (qq.y)/2) {
                        continue;
                    }
                    r->h = (qq.y - p->y)/2;
                }
                rotate90(&dx, &dy);
                k++;
            }
        } while (k < 2);
    }

    rooms = (Room*)realloc(rooms, sizeof(Room)*n_rooms);

    //int _tile = 0;
    //int tiles[7] = { 1, 4,5,7,11,100,102};
    //for (i=0; i<n_rooms; i++) {
        //rooms[i].draw(z, tiles[_tile]);
        //_tile++;
        //_tile%=7;
    //}

    find_duplicate_corners(corners, n_corners);
}

class RoomEdge {
    public:
    int room;
    int n;
    int *rooms;

    void set_room(int id) {
        this->room = id;
    }

    int has(int id) {
        int i;
        for (i=0; i<n; i++) {
            if (this->rooms[i] == id) return i;
        }
        return -1;
    }

    void add_multiple(int *ids, int n_ids) {
        if (n_ids <= 0) return;
        int new_n = this->n + n_ids;
        if (this->rooms == NULL) {
            this->rooms = (int*)malloc(sizeof(int)*new_n);
        } else {
            this->rooms = (int*)realloc(this->rooms, sizeof(int)*new_n);
        }

        int i;
        for (i=0; i<n_ids; i++) {
            if (this->has(ids[i]) < 0 && !(this->room = ids[i])) {
                this->n++;
                this->rooms[this->n] = ids[i];
            }
        }

        if (this->n != new_n) {
            this->rooms = (int*)realloc(this->rooms, sizeof(int)*this->n);
        }

    }

    void add(int id) {
        if (id < 0) return;
        if (id == this->room) return;
        if (this->has(id) >= 0) return;
        this->n++;
        if (this->rooms == NULL) {
            this->rooms = (int*)malloc(sizeof(int)*this->n);
        } else {
            this->rooms = (int*)realloc(this->rooms, sizeof(int)*this->n);
        }
        this->rooms[n-1] = id;
    }

    void remove(int id) {
        if (id < 0) return;
        if (id == this->room) return;
        int index;
        index = this->has(id);
        if (index < 0) return;
        if (this->rooms == NULL) return;
        if (n == 0) return;

        this->n--;
        int i;
        for (i=index; i<this->n; i++) {
            this->rooms[i] = this->rooms[i+1];
        }
        
        this->rooms = (int*)realloc(this->rooms, sizeof(int)*this->n);
    }

    void teardown() {
        if (this->rooms != NULL) {
            free(this->rooms);
            this->rooms = NULL;
        }
    }

    void print() {
        if (this->rooms == NULL) return;
        int i;
        for (i=0; i<this->n; i++) {
            printf("%d", this->rooms[i]);
            printf("\n");
        }
    }

    RoomEdge() :
    n(0),
    rooms(NULL)
    {}

    ~RoomEdge()
    {
        if (this->rooms != NULL) {
            free(this->rooms);
            this->rooms = NULL;
        }
    }
};

RoomEdge* room_edges = NULL;

void build_adjacency_graph() {

    // 2 rooms share an edge if one of their bounding lines share a line segment
    int i,j;

    if (room_edges != NULL) {
        for (i=0; i<n_room_edges; i++) {
            room_edges[i].teardown();
        }
        free(room_edges);
    }
    n_room_edges = n_rooms;

    room_edges = (RoomEdge*)malloc(sizeof(RoomEdge)*n_room_edges);
    for (i=0; i<n_room_edges; i++) {
        room_edges[i].n = 0;
        room_edges[i].rooms = NULL;
        room_edges[i].room = i;
        rooms[i].edges = i;
    }
    
    Room *r,*s;
    RoomEdge *re,*se;
    for (i=0; i<n_rooms; i++) {
        r = &rooms[i];
        re = &room_edges[i];
        r->edges = i;
        //re->room = i;
        for (j=0; j<n_rooms; j++) {
            if (i==j) continue;
            s = &rooms[j];
            if (r->shares_edge_with(s)) {
                se = &room_edges[j];
                //s->edges = j;
                //se->room = j;
                re->add(j);
                se->add(i);
            }
        }
    }

    printf("%d rooms found\n", n_rooms);

    //for (i=0; i<n_rooms; i++) {
        //printf("Room %d: \n", i);
        //r = &rooms[i];
        //re = &room_edges[r->edges];
        //re->print();
        //printf("\n");
    //}
}

//void draw() {
    //int i,j;
    //Point *p, *q;
    //Diagonal d;
    //int z = 14;

    //// draw diagonals
    //if (diagonals != NULL) {
        //for (i=0; i<n_diagonals; i++) {
            //d = diagonals[i];
            //p = &points[d.p];
            //q = &points[d.q];
            //_draw_line(255,10,10, ((float)p->x)/2+0.5, ((float)p->y)/2+0.5, z+0.1, ((float)q->x)/2+0.5, ((float)q->y)/2+0.5, z+0.1);
        //}
    //}

    ////// draw corners
    ////if (corners != NULL) {
        ////for (i=0; i<n_corners; i++) {
            ////p = &corners[i];
            ////_draw_line(255,10,10, ((float)p->x)/2+0.5, ((float)p->y)/2+0.5, z+0.1, (float)p->bx + 0.5, (float)p->by + 0.5, z+0.1);
        ////}
    ////}

    //Room *r,*s;
    //RoomEdge *re;
    //// draw adjacency graph
    //if (room_edges != NULL) {
        //for (i=0; i<n_rooms; i++) {
            //r = &rooms[i];
            //re = &room_edges[r->edges];
            //// draw line from center of room to center of adjacent room
            //for (j=0; j<re->n; j++) {
                //s = &rooms[re->rooms[j]];
                //_draw_line(255,10,10, r->center_fx(), r->center_fy(), z+0.1, s->center_fx(), s->center_fy(), z+0.1);
            //}
        //}
    //}
//}

void place_rooms(Room *rooms, int* current, int n_current, int z, int clique) {
    int i;
    Room *r;
    for (i=0; i<n_current;i++) {
        r = &rooms[current[i]];
        if (r->placed) continue;
        r->z = z;
        r->clique = clique;
        r->placed = true;
    }
}

// returns suggested initial z level
// inputs: first room, ceiling z of first room
int get_inital_z_level(Room* room, int z_top) {
    return z_top - room->d - 1;
}

inline bool int_in_array(int i, int *arr, int arr_size) {
    int k;
    for (k=0; k<arr_size;k++) {
        if (arr[k] == i) return true;
    }
    return false;
}

void get_adjacent_rooms(Room *rooms, RoomEdge *room_edges, int *current, int *next, int *n_current) {
    int i,j;
    int n = 0;
    Room *r,*s;
    RoomEdge *re;
    int adj;
    for (i=0; i<*n_current; i++) {
        r = &rooms[current[i]];
        re = &room_edges[r->edges];
        for (j=0; j<re->n; j++) {
            adj = re->rooms[j];
            s = &rooms[adj];
            if (s->placed) continue;
            if (int_in_array(adj, next, n)) continue;
            next[n++] = adj;
        }
    }
    *n_current = n;
}

void rooms_not_placed(Room* rooms, int n_rooms, int* not_placed, int *n_not_placed) {
    int i;
    int n = 0;
    for (i=0; i<n_rooms; i++) {
        if (rooms[i].placed) continue;
        not_placed[n++] = i;
    }
    *n_not_placed = n;
}

int choose_unplaced_room(Room *rooms, int n_rooms, int *not_placed, int n_not_placed) {
    int id = randrange(0, n_not_placed-1);
    id = not_placed[id];
    return id;
}

class Clique {
    public:
    int length;
    int size;
};

Clique* cliques = NULL;
int n_cliques = 0;

void set_room_z_levels(int z_top) {

    // breadth first search
    
    if (!n_rooms) return;

    if (cliques != NULL) {
        free(cliques);
        cliques = NULL;
        n_cliques = 0;
    }
    
    int room_height = 7;
    int i;
    for (i=0; i<n_rooms; i++) {
        rooms[i].placed = false;
        rooms[i].z = 0;
        //rooms[i].d = room_height;
        rooms[i].d = randrange(room_height-2, room_height);
        rooms[i].clique = -1;
    }

    int *not_placed = (int*)malloc(sizeof(int)*n_rooms);
    int n_not_placed = n_rooms;
    rooms_not_placed(rooms, n_rooms, not_placed, &n_not_placed);
    if (n_rooms != n_not_placed) {
        printf("WARNING: initial rooms_not_placed scan finds %d rooms not placed, out of %d\n", n_not_placed, n_rooms);
    }
    int *current = (int*)malloc(sizeof(int)*n_rooms);
    int *next = (int*)malloc(sizeof(int)*n_rooms);
    int n_current = 0;
    int n_current_prev = 0;

    // first room, anchor it
    current[n_current++] = choose_unplaced_room(rooms, n_rooms, not_placed, n_not_placed);
    int z = get_inital_z_level(&rooms[current[0]], z_top);
//printf("Initial z-level: ");
    int length = n_current;
    cliques = (Clique*)malloc(sizeof(Clique)*n_rooms);
    n_cliques = 0;
    while (n_not_placed) {
        if (!n_current) {
            // advance clique, lengths
            cliques[n_cliques].length = length;
            printf("Clique %d length %d\n", n_cliques, length);
            cliques[n_cliques].size = n_current_prev;
            n_cliques++;

            // get new room
            current[n_current++] = choose_unplaced_room(rooms, n_rooms, not_placed, n_not_placed);
            length = n_current;
        }

        place_rooms(rooms, current, n_current, z, n_cliques);
        z -= randrange(0,1);
        z = (z < 1) ? 1 : z;

        n_current_prev = n_current;
        get_adjacent_rooms(rooms, room_edges, current, next, &n_current);
        memcpy(current, next, sizeof(int)*n_rooms);
        length += n_current;

        rooms_not_placed(rooms, n_rooms, not_placed, &n_not_placed);
    }

    cliques[n_cliques].length = length;
    printf("Clique %d length %d\n", n_cliques, length);
    cliques[n_cliques].size = n_current_prev;
    n_cliques++;
    printf("%d cliques found\n", n_cliques);
    cliques = (Clique*)realloc(cliques, sizeof(Clique)*n_cliques);

    Clique* c;
    int longest = -1;
    int longest_clique;
    for (i=0; i<n_cliques; i++) {
        c = &cliques[i];
        if (c->length > longest) {
            longest = c->length;
            longest_clique = i;
        }
    }

    printf("Longest clique: %d, length: %d\n", longest_clique, longest);

    // calculate depth of the largest clique
    Room *r;
    Room *top;
    int lowest_z = 100000;
    int highest_z = -1;
    for (i=0; i<n_rooms; i++) {
        r = &rooms[i];
        if (r->clique != longest_clique) continue;
        if (r->z > highest_z) {
            top = r;
            highest_z = r->z;
        }
        lowest_z = (r->z < lowest_z) ? r->z : lowest_z;
    }
    highest_z = top->d + top->z;

    // rearrange the smaller cliques to fit within this range
    int j;
    int z_off;
    int highest_z_inner;
    int highest_z_inner_height;
    int clique_z;
    for (i=0; i<n_cliques; i++) {
        if (i == longest_clique) continue;

        highest_z_inner = -1;
        highest_z_inner_height = 0;

        for (j=0; j<n_rooms; j++) {
            r = &rooms[j];
            if (r->clique != i) continue;
            if (r->z + r->d > highest_z_inner) {
                highest_z_inner = r->z;
                highest_z_inner_height = r->d;
            }
        }

        if (lowest_z > highest_z - highest_z_inner_height - 1)printf("WARNING clique range will fail\n");
        clique_z = randrange(lowest_z, highest_z - highest_z_inner_height - 1);
        z_off = clique_z - highest_z_inner;
        for (j=0; j<n_rooms; j++) {
            r = &rooms[j];
            if (r->clique != i) continue;
            r->z += z_off;
        }
    }
    
    free(current);
    free(next);
    free(not_placed);
}

void draw_rooms(Room* rooms, int n_rooms) {
    int i;
    for (i=0; i<n_rooms; i++) {
        rooms[i].draw_3d(0);
    }
}
void draw_rooms(Room* rooms, int n_rooms, Room* highest, int *highest_room_z, Room* lowest, int *lowest_room_z) {
    int i;
    int highest_z = -100;
    int lowest_z = 10000;
    Room *r;
    Room *high_r=NULL, *low_r=NULL;
    for (i=0; i<n_rooms; i++) {
        rooms[i].draw_3d(0);
        r = &rooms[i];
        if (r->z + r->d > highest_z) {
            highest_z = r->z + r->d;
            high_r = r;
        }
        if (r->z < lowest_z) {
            lowest_z = r->z;
            low_r = r;
        }
    }
    if (high_r != NULL) *highest = *high_r;
    if (low_r != NULL) *lowest = *low_r;
    *highest_room_z = highest_z;
    *lowest_room_z = lowest_z;
}

void generate_dungeon(int z, int tile) {
    rect_solver(z);
    resolve_rooms(z, tile);
    build_adjacency_graph();
    
    z=127;
    set_room_z_levels(z);

    int i,j,k;
    int depth = 128;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            for (k=0; k<depth; k++) {
                _set(i,j,k,101);
            }
        }
    }

    //Room *highest = NULL, *lowest = NULL;
    Room highest, lowest;
    int highest_room_z = -100, lowest_room_z = 10000;
    draw_rooms(rooms, n_rooms, &highest, &highest_room_z, &lowest, &lowest_room_z);
printf("Highest: %d\n", highest_room_z);
printf("Lowest: %d\n", lowest_room_z);
    //Room *highest;
    //int highest_room_z = -100;
    //for (i=0; i<n_rooms; i++) {
        //rooms[i].draw_3d(0);
        //highest = &rooms[i];
        //if (highest->z + highest->d > highest_room_z) {
            //highest_room_z = highest->z + highest->d;
        //}
    //}

    int si=-1,sj=-1;
    //if (highest != NULL)
    //{
        si = (int)highest.center_fx();
        sj = (int)highest.center_fy();
    //}


    //int n_rooms2 = n_rooms;
    //Room *rooms2 = (Room*)malloc(sizeof(Room) * n_rooms);
    //memcpy(rooms2, rooms, sizeof(Room)*n_rooms);
    
// generate 2nd dungeon, then link

//switch seeds
// next_seed(); //???
    srand(70777);

// draw on highest z level
    z = 127;
    printf("Generate dragon2\n");
    Dragon::generate(z);

// process, with highest z level
printf("Solve rect2\n");
    rect_solver(z);
printf("Resolve rooms2\n");
    resolve_rooms(z, tile);
printf("Build adjacency graph2\n");
    build_adjacency_graph();
printf("set room z levels2\n");
    set_room_z_levels(lowest_room_z - 2);

// draw rooms
printf("draw rooms2\n");

    Room highest2, lowest2;
    int highest_room_z2 = -100, lowest_room_z2 = 10000;
    draw_rooms(rooms, n_rooms, &highest2, &highest_room_z2, &lowest2, &lowest_room_z2);
    //draw_rooms(rooms, n_rooms);

// connect rooms
i = lowest.z;
int end;
int cx,cy;
cx = lowest.center_fx();
cy = lowest.center_fy();
while(i >= 0) {
    i--;
    if (_get(cx, cy, i) == 0) break;
}
if (i >= 0) {
    end = i;
    for (i=lowest.z; i>end; i--) {
        _set(cx, cy, i, 0);
    }
} else {
    printf("FAILED\n");
    // connect rooms
    i = highest2.z + highest2.d;
    int end;
    cx = highest2.center_fx();
    cy = highest2.center_fy();
    while(i < zmax) {
        i++;
        if (_get(cx,cy, i) == 0) break;
    }
    if (i < zmax) {
        end = i;
        for (i=highest2.z+highest.d; i<end; i++) {
            _set(cx,cy, i, 0);
        }
    } else printf("FAILED2\n");
}

printf("box it in\n");
    _box(128,128,0,127,101);
printf("make dungeon entrance\n");
printf("highest_room_z: %d\n", highest_room_z);
printf("si,sj %d,%d\n", si,sj);
    if (si >= 0 && si < xmax && sj >= 0 && sj < ymax) {
        for (i=127; i>=highest_room_z-1; i--) {
            printf("%d\n", i);
            _set(si,sj,i,0);
        }
    }

}

}
