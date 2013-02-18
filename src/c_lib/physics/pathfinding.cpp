#include "pathfinding.hpp"

#include <stdlib.h>
#include <t_map/_interface.hpp>
#include <common/common.hpp>
#if DC_CLIENT
# include <SDL/draw_functions.hpp>
#endif

namespace Path
{

/*
 * References:
 *
 * http://www.policyalmanac.org/games/aStarTutorial.htm
 * http://www-cs-students.stanford.edu/~amitp/gameprog.html#paths
 * http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
 * http://aigamedev.com/open/tutorial/symmetry-in-pathfinding/
 * http://grastien.net/ban/articles/hg-aaai11.pdf
 *
 */

// F = G + H
// F: final cost
// G: movement cost (10 for lateral, 14 for diagonal)
// H: heuristic (manhattan)

static const size_t OPEN_START_SIZE = 128;
static const size_t CLOSED_START_SIZE = 128;

struct Adjacency
{
    int x,y,z;
    int cost;
};

static const struct Adjacency adj[8+9+9] = {
    // xy
    {  1,  0,  0, 10 },
    { -1,  0,  0, 10 },
    {  0,  1,  0, 10 },
    {  0, -1,  0, 10 },
    {  1,  1,  0, 14 },
    {  1, -1,  0, 14 },
    { -1,  1,  0, 14 },
    { -1, -1,  0, 14 },
    // xy +
    {  1,  0,  1, 14 },
    { -1,  0,  1, 14 },
    {  0,  1,  1, 14 },
    {  0, -1,  1, 14 },
    {  1,  1,  1, 18 },
    {  1, -1,  1, 18 },
    { -1,  1,  1, 18 },
    { -1, -1,  1, 18 },
    {  0,  0,  1, 10 },
    // xy -z
    {  1,  0, -1, 14 },
    { -1,  0, -1, 14 },
    {  0,  1, -1, 14 },
    {  0, -1, -1, 14 },
    {  1,  1, -1, 18 },
    {  1, -1, -1, 18 },
    { -1,  1, -1, 18 },
    { -1, -1, -1, 18 },
    {  0,  0, -1, 10 },
};

struct Score
{
    float g;
    float h;
    ALWAYS_INLINE float f()
    {
        return g + h;
    }
};

float euclidean_distance(const struct MapPos& start, const struct MapPos& end)
{
    int x = quadrant_translate_i(end.x, start.x) - end.x;
    int y = quadrant_translate_i(end.y, start.y) - end.y;
    return sqrtf(x*x + y*y) * 10;
}

float diagonal_distance(const struct MapPos& start, const struct MapPos& end)
{
    int x = quadrant_translate_i(end.x, start.x) - end.x;
    int y = quadrant_translate_i(end.y, start.y) - end.y;
    return GS_MAX(abs(x), abs(y)) * 10;
}

struct Node
{
    struct MapPos pos;
    struct Score score;
    int id;
    int parent;

    void _update_score(float g, const struct MapPos& end)
    {
        this->score.g += g;
        this->score.h = euclidean_distance(this->pos, end);
    }

    void first(const struct MapPos& start, const struct MapPos& end)
    {   // init first node in the sequence
        this->pos = start;
        this->parent = -1;
        this->id = 0;
        this->_update_score(0, end);
    }

    void update(const struct Adjacency& adj, const struct MapPos& end)
    {
        this->pos.x = translate_point(this->pos.x + adj.x);
        this->pos.y = translate_point(this->pos.y + adj.y);
        this->pos.z = translate_point(this->pos.z + adj.z);
        this->_update_score(adj.cost, end);
    }

    bool reassign(float g, int parent)
    {   // if the new g score is better, take that parent
        if (g >= this->score.g)
            return false;
        this->score.g = g;
        this->parent = parent;
        return true;
    }

    void print()
    {
        //printf("ID: %d; Parent: %d; Score: g=%d, h=%d; ", this->id,
               //this->parent, this->score.g, this->score.h);
        print_pos(this->pos);
    }
};

static int compare_node_score(const void* a, const void* b)
{   // we want descending order, so b comes first
    float d = ((struct Node*)b)->score.f() - ((struct Node*)a)->score.f();
    // we do a tiebreaker here, to reduce the search space. if the point is closer,
    // according to our heuristic h, then it wins in the tie
    if (d == 0)
        d = ((struct Node*)b)->score.h - ((struct Node*)a)->score.h;
    return int(ceil_from_zero(d));
}

static ALWAYS_INLINE void sort_nodes(struct Node* nodes, size_t len)
{
    qsort(nodes, len, sizeof(struct Node), &compare_node_score);
}

static inline int add_node(struct Node& node, struct Node*& nodes,
                            size_t& max_len, size_t& len)
{
    if (len >= max_len)
    {
        len = max_len;
        struct Node* _nodes = (struct Node*)realloc(nodes, len*2*sizeof(struct Node));
        IF_ASSERT(_nodes == NULL) return -1;
        nodes = _nodes;
        max_len *= 2;
    }
    node.id = len;
    nodes[len++] = node;
    return len;
}

static inline int get_node_pos_index(const struct MapPos& pos,
                                     const Node* nodes, size_t len)
{
    for (size_t i=0; i<len; i++)
        if (is_equal(pos, nodes[i].pos))
            return int(i);
    return -1;
}

struct MapPos* get_path(const struct MapPos& start,
                        const struct MapPos& end, size_t& len)
{
    IF_ASSERT(!is_boxed_position(start) || !is_boxed_position(end)) return NULL;
    len = 0;
    // hacks to prevent lockup until 3D is implemented
    if (start.z != end.z) return NULL;
    if (t_map::isSolid(end)) return NULL;

    // TODO --
    // 3d
    // Global open/closed buffer, init/teardown (minimize allocations)
    // sqrtf lookups
    // different sized bounding boxes
    // handle corner blocks properly (cant cut through open diagonal, must walk around corners)
    // prevent nasty crash/segfault when putting pointer far away (from somewhere to 0,0)

    printf("Finding path from:\n");
    printf("\t");
    print_pos(start);
    printf("to\n");
    printf("\t");
    print_pos(end);

    size_t iopen = 0;
    size_t iclosed = 0;
    size_t mopen = OPEN_START_SIZE;
    size_t mclosed = CLOSED_START_SIZE;
    struct Node* open = (struct Node*)malloc(mopen * sizeof(struct Node));
    struct Node* closed = (struct Node*)malloc(mclosed * sizeof(struct Node));
    open[iopen++].first(start, end);
    bool needs_sort = false;

    while (1)
    {
        // if the open list is empty, then we failed to find a path at all
        if (iopen == 0)
            break;
        if (needs_sort)
        {
            sort_nodes(open, iopen);
            needs_sort = false;
        }
        // get lowest cost node list
        struct Node current = open[--iopen];

        // move to closed list
        add_node(current, closed, mclosed, iclosed);
        // if we found the target, break
        if (is_equal(current.pos, end))
            break;

        for (int i=0; i<8; i++)
        {   // iterate adjacent squares
            struct Node node = current;
            node.update(adj[i], end);

            // if blocked or in closed list, skip
            if (t_map::isSolid(node.pos) ||
                get_node_pos_index(node.pos, closed, iclosed) >= 0)
            {
                continue;
            }

            int in_open = get_node_pos_index(node.pos, open, iopen);
            if (in_open < 0)
            {   // not found; add to open list
                node.parent = current.id;
                add_node(node, open, mopen, iopen);
                needs_sort = true;
            }
            else
            {   // update if g is better
                needs_sort = open[in_open].reassign(node.score.g, current.id);
            }
        }
    }

    static CubeType mushroom = t_map::get_cube_type("mushroom_cap2");
    for (size_t i=0; i<iopen; i++)
    {
        struct MapPos pos = open[i].pos;
        pos.z = t_map::map_dim.z - 1;
        t_map::set(pos, mushroom);
    }
    for (size_t i=0; i<iclosed; i++)
    {
        struct MapPos pos = closed[i].pos;
        pos.z = t_map::map_dim.z - 1;
        t_map::set(pos, mushroom);
    }

    len = 0;
    struct MapPos* path = NULL;
    if (iopen != 0)
    {   // walk path backwards, from last item on closed list
        // count length of path, allocate
        len = 1;
        closed[0].parent = -1;
        int first = closed[iclosed-1].parent;
        while (first >= 0)
        {
            first = closed[first].parent;
            len++;
        }
        path = (struct MapPos*)malloc(len * sizeof(struct MapPos));

        // add to path array
        int i = len;
        first = closed[iclosed-1].id;
        while (first >= 0)
        {
            path[--i] = closed[first].pos;
            first = closed[first].parent;
        }
    }

    // teardown
    free(open);
    free(closed);
    return path;
}

void print_path(const struct MapPos* path, size_t len)
{
    if (path == NULL) return;
    for (size_t i=0; i<len; i++)
    {
        printf("%d: ", int(i));
        print_pos(path[i]);
    }
}

#if DC_CLIENT
void draw_path(const struct MapPos* path, size_t len)
{
    if (path == NULL) return;
    if (len < 2) return;
    const Color color = Color(0, 220, 0);
    const struct Vec3 offset = vec3_init(0.5f, 0.5f, 0.05f);
    for (size_t i=0; i<len-1; i++)
    {
        struct Vec3 a = vec3_add(vec3_init(path[i]), offset);
        struct Vec3 b = vec3_add(vec3_init(path[i+1]), offset);
        a.z = 128.05f;
        b.z = 128.05f;
        a = quadrant_translate_position(current_camera_position, a);
        b = quadrant_translate_position(current_camera_position, b);
        draw_line(color, a, b);
    }
}
#endif

}   // Path
